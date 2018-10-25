#include "moc_deps.h"
#include "QWebRTCProxy.h"
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
  sslConfiguration.setProtocol(QSsl::TlsV1SslV3);
  webSocket_.setSslConfiguration(sslConfiguration);

  connect(this, &QSfuSignaling::sendMessgeToSfu, this, &Controller::onSendMessgeToSfu);
  connect(this, &QSfuSignaling::commandFinished, this, &Controller::onCommandFinished);
  connect(&webSocket_, &QWebSocket::connected, this, &Controller::onConnectedSfu);
  connect(&webSocket_, &QWebSocket::disconnected, this, &Controller::onDisconnectedSfu);
  connect(&webSocket_, &QWebSocket::textMessageReceived, this, &Controller::onReceivedSfuMessage);
  connect(&webSocket_, &QWebSocket::sslErrors, this, &Controller::onSslErrors);
  connect(&webSocket_, &QWebSocket::stateChanged, this, &Controller::onStateChanged);
}

Controller::~Controller() {
  delete webrtcProxy_;
}

bool Controller::connectedSfu() const
{
  return connectedSfu_;
}

void Controller::createOffer()
{
  auto peerConnection = webrtcProxy_->createPeerConnection();
  peerConnection->createOffer();
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
  qDebug("[%s]", __func__);
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
  // TODO
//  for (auto &e : errors) {
//    qDebug() << "\t" << e.errorString();
//  }
}

void Controller::onSendMessgeToSfu(const std::string& message)
{
  qDebug("[%s] msg=\"%s\"", __func__, message.c_str());
  webSocket_.sendTextMessage(QString::fromStdString(message));
}

void Controller::onCommandFinished(const std::string &cmd, const std::string& result)
{
  qDebug("[%s] cmd: %d, result:%s", __func__, cmd, result.c_str());
}

