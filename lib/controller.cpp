#include "moc_deps.h"
#include "QWebRTCProxy.h"
#include "MediaStreamProxy.h"
#include "controller.h"

#include <QDebug>

Controller::Controller(QObject *parent) :
  QSfuSignaling(parent),
  webrtcProxy_(QWebRTCProxy::getInstance())
{
  QSslConfiguration sslConfiguration;
  QFile certFile(QStringLiteral(":/certs/client.pem"));
  QFile keyFile(QStringLiteral(":/certs/client.key"));
  QFile caFile(QStringLiteral(":/certs/ca_server.pem"));
  certFile.open(QIODevice::ReadOnly);
  keyFile.open(QIODevice::ReadOnly);
  caFile.open(QIODevice::ReadOnly);
  QSslCertificate certificate(&certFile, QSsl::Pem);
  QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
  QSslCertificate caCertificate(&caFile, QSsl::Pem);
  certFile.close();
  keyFile.close();
  caFile.close();
  sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
  sslConfiguration.setLocalCertificate(certificate);
  auto caCerts = sslConfiguration.caCertificates();
  caCerts.append(caCertificate);
  sslConfiguration.setPrivateKey(sslKey);
  sslConfiguration.setCaCertificates(caCerts);
  sslConfiguration.setProtocol(QSsl::TlsV1_2);
  webSocket_.setSslConfiguration(sslConfiguration);

  connect(this, &QSfuSignaling::streamPublishedEvent, this, &Controller::onStreamPublished);
  connect(this, &QSfuSignaling::streamUnpublishedEvent, this, &Controller::onStreamUnpublished);
  connect(this, &QSfuSignaling::participantJoinedEvent, this, &Controller::onParticipantJoined);
  connect(this, &QSfuSignaling::participantLeftEvent, this, &Controller::onParticipantLeft);
  connect(this, &QSfuSignaling::participantKickedEvent, this, &Controller::onParticipantKicked);
  connect(this, &QSfuSignaling::activeSpeakerChangedEvent, this, &Controller::onActiveSpeakerChanged);
  connect(&webSocket_, &QWebSocket::connected, this, &Controller::onConnectedSfu);
  connect(&webSocket_, &QWebSocket::disconnected, this, &Controller::onDisconnectedSfu);
  connect(&webSocket_, &QWebSocket::textMessageReceived, this, &Controller::onReceivedSfuMessage);
  connect(&webSocket_, &QWebSocket::sslErrors, this, &Controller::onSslErrors);
  connect(&webSocket_, &QWebSocket::stateChanged, this, &Controller::onStateChanged);

  peerConnection_ = webrtcProxy_->createPeerConnection();
}

Controller::~Controller() {
  delete webrtcProxy_;
}

Controller::State Controller::getState() const
{
  return state;
}

void Controller::connectSfu(const std::string& sfuUrl, const std::string& clientId)
{
  qDebug("[%s] url=%s, id=%s", __func__, sfuUrl.c_str(), clientId.c_str());

  QString ws = QString::fromStdString(sfuUrl + "/?clientId=" + clientId);
  webSocket_.open(QUrl(ws));
}

void Controller::disconnectSfu()
{
  qDebug("[%s]", __func__);
  webSocket_.close();
}

void Controller::onConnectedSfu()
{
  Log("Successfully connect to SFU");
  state = State::Connected;
}

void Controller::onDisconnectedSfu()
{
  qDebug("[%s]", __func__);
  state = State::Disconnected;
}

void Controller::onStateChanged(QAbstractSocket::SocketState state)
{
  qDebug("[%s] state=%d", __func__, state);
  switch (state) {
    case QAbstractSocket::SocketState::UnconnectedState:
      Log("WS:Unconnected");
      break;
    case QAbstractSocket::SocketState::HostLookupState:
      Log("WS:HostLookup");
      break;
    case QAbstractSocket::SocketState::ConnectingState:
      Log("WS:Connecting");
      break;
    case QAbstractSocket::SocketState::ConnectedState:
      Log("WS:Connected");
      break;
    case QAbstractSocket::SocketState::BoundState:
      Log("WS:Bound");
      break;
    case QAbstractSocket::SocketState::ListeningState:
      Log("WS:Listening");
      break;
    case QAbstractSocket::SocketState::ClosingState:
      Log("WS:Closing");
      break;
  }
}

void Controller::onSslErrors(const QList<QSslError> &errors)
{
  Log("Got ssl Errors:");
  for (auto &e : errors) {
    Log("\t" + e.errorString().toStdString());
  }
}

void Controller::send(const std::string &message)
{
  qDebug("[%s] msg=\"%s\"", __func__, message.c_str());
  webSocket_.sendTextMessage(QString::fromStdString(message));
}

void Controller::onStreamPublished()
{
  qDebug("[%s]", __func__);
}
void Controller::onStreamUnpublished(const std::string &streamId)
{
  qDebug("[%s]", __func__);
}
void Controller::onParticipantJoined(const std::string &roomId, const std::string &clientId, const std::string &reason)
{
  qDebug("[%s]", __func__);
}
void Controller::onParticipantLeft(const std::string &roomId, const std::string &clientId, const std::string &reason)
{
  qDebug("[%s]", __func__);
}
void Controller::onParticipantKicked(const std::string &roomId, const std::string &reason)
{
  qDebug("[%s]", __func__);
}
void Controller::onActiveSpeakerChanged(const std::string &roomId, const std::string &clientId)
{
  qDebug("[%s]", __func__);

}

void Controller::onPublishedStream(bool success)
{
  qDebug("[%s]", __func__);
}

void Controller::joinRoom()
{
  peerConnection_->disconnect(SIGNAL(oncreateoffersuccess));
  connect(peerConnection_, &PeerConnectionProxy::oncreateoffersuccess,
          this, &Controller::onCreatedJoinRoomOfferSuccess);
  peerConnection_->createOffer();
}

void Controller::publishCamera()
{
  state = State::Camera;
  auto videoTrack = webrtcProxy_->createVideoCapturer()->getVideoTrack();
  auto audioTrack = webrtcProxy_->createAudioCapturer()->getAudioTrack();

  auto stream = webrtcProxy_->createLocalStream("camera");
  stream->addTrack(videoTrack);
  stream->addTrack(audioTrack);
  peerConnection_->addStream(stream);
  // TODO: render stream
  peerConnection_->disconnect(SIGNAL(oncreateoffersuccess));
  connect(peerConnection_, &PeerConnectionProxy::oncreateoffersuccess,
          this, &Controller::onCreatedPublishCameraOfferSuccess);
  peerConnection_->createOffer();
}

void Controller::publishDesktop()
{
  state = State::Desktop;
}

void Controller::Log(const std::string &log)
{
  if (logger) logger(log);
}

void Controller::onCreatedJoinRoomOfferSuccess(const QJsonObject &sdp)
{
  qDebug("[%s]", __func__);
  peerConnection_->setLocalDescription(sdp);
  std::string sdp_ = sdp.value("sdp").toString().toStdString();
  auto offerInfo = SDPInfo::parse(sdp_);
  sfu_->join(roomId_, roomAccessPin_, offerInfo,
                  [this](const dm::Participant::Joined &r) {
    if (!r.error) {
      sdpInfo_ = r.result->sdpInfo;
      for (auto stream : r.result->streams) {
        sdpInfo_->addStream(stream);
      }

      QJsonObject desc;
      desc["type"] = QString("answer");
      desc["sdp"]  = QString::fromStdString(sdpInfo_->toString());
      peerConnection_->setRemoteDescription(desc);

      // FIXME: crash on mac
//      std::vector<dm::VideoProfile> profiles = {
//          {"camera", dm::LayerTraversalAlgorithm::ZigZagSpatialTemporal},
//          {"screenshare", dm::LayerTraversalAlgorithm::SpatialTemporal},
//      };
//      sfu_->setProfiles(roomId_, profiles, [](...){
//          Log("Profiles set");
//      });
    }
    Log("Join Room " + r.toString());
  });
}

void Controller::onCreatedPublishCameraOfferSuccess(const QJsonObject &sdp)
{
  qDebug("[%s]", __func__);
  peerConnection_->setLocalDescription(sdp);
  std::string sdp_ = sdp.value("sdp").toString().toStdString();
  connect(this, &QSfuSignaling::publishedStream, this, &Controller::onPublishedStream);
  publishStream(sdp_, state == State::Camera ? true : false);
}

void Controller::onCreatedPublishDesktopOfferSuccess(const QJsonObject &sdp)
{

}

void Controller::onGotICECandidate(const QJsonObject &candidate)
{
  qDebug("[%s]", __func__);
}
void Controller::onAddedStream(MediaStreamProxy* stream)
{
  qDebug("[%s]", __func__);
}
void Controller::onSetRemoteDescriptionSuccess()
{
  qDebug("[%s]", __func__);
}
void Controller::onCreatedAnswerSuccess(const QJsonObject &sdp)
{
  qDebug("[%s]", __func__);
}
