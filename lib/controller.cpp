#include "moc_deps.h"
#include "QWebRTCProxy.h"
#include "MediaStreamProxy.h"
#include "controller.h"

#include <QDebug>

#define LOG Q_EMIT showLog

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

  connect(this, &QSfuSignaling::sendMessgeToSfu, this, &Controller::onSendMessgeToSfu);
  connect(this, &QSfuSignaling::sendSfuLog, this, &Controller::onGotSfuLog);
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

bool Controller::connectedSfu() const
{
  return connectedSfu_;
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
  LOG("Successfully connect to SFU");
  connectedSfu_ = true;
}

void Controller::onDisconnectedSfu()
{
  qDebug("[%s]", __func__);
  connectedSfu_ = false;
}

void Controller::onStateChanged(QAbstractSocket::SocketState state)
{
  qDebug("[%s] state=%d", __func__, state);
}

void Controller::onSslErrors(const QList<QSslError> &errors)
{
  qDebug("[%s] num:%d", __func__, errors.size());
  LOG("Got ssl Errors:");
  for (auto &e : errors) {
    LOG("\t" + e.errorString());
  }
}

void Controller::onSendMessgeToSfu(const std::string& message)
{
  qDebug("[%s] msg=\"%s\"", __func__, message.c_str());
  webSocket_.sendTextMessage(QString::fromStdString(message));
}

void Controller::onGotSfuLog(const std::string& log)
{
  Q_EMIT showLog(QString::fromStdString(log));
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
    joinedRoom_ = true;
}
void Controller::onParticipantLeft(const std::string &roomId, const std::string &clientId, const std::string &reason)
{
    qDebug("[%s]", __func__);
    joinedRoom_ = false;
}
void Controller::onParticipantKicked(const std::string &roomId, const std::string &reason)
{
    qDebug("[%s]", __func__);
    joinedRoom_ = false;
}
void Controller::onActiveSpeakerChanged(const std::string &roomId, const std::string &clientId)
{
    qDebug("[%s]", __func__);

}

void Controller::createOffer()
{
  connect(peerConnection_, &PeerConnectionProxy::oncreateoffersuccess,
          this, &Controller::onCreatedOfferSuccess);
  peerConnection_->createOffer();
}

void Controller::publishCamera()
{
  auto videoTrack = webrtcProxy_->createVideoCapturer()->getVideoTrack();
  auto audioTrack = webrtcProxy_->createAudioCapturer()->getAudioTrack();

  auto stream = webrtcProxy_->createLocalStream("camera");
  stream->addTrack(videoTrack);
  stream->addTrack(audioTrack);

}

void Controller::publishDesktop()
{

}

void Controller::onCreatedOfferSuccess(const QJsonObject &sdp)
{
  qDebug("[%s]", __func__);
  peerConnection_->setLocalDescription(sdp);
  connect(this, &QSfuSignaling::gotAnswerInfo, this,
          &Controller::onGotAnswerInfo);
  std::string sdp_ = sdp.value("sdp").toString().toStdString();
  joinRoom(sdp_);
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

void Controller::onGotAnswerInfo(const std::string &sdp)
{
  qDebug("[%s] dsp=%s", __func__, sdp.c_str());

  QJsonObject desc;
  desc["type"] = QString("answer");
  desc["sdp"]  = QString::fromStdString(sdp);
  peerConnection_->setRemoteDescription(desc);
}
