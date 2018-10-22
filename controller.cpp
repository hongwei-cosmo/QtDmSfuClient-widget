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
  certFile.open(QIODevice::ReadOnly);
  keyFile.open(QIODevice::ReadOnly);
  QSslCertificate certificate(&certFile, QSsl::Pem);
  QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
  certFile.close();
  keyFile.close();
  sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
  sslConfiguration.setLocalCertificate(certificate);
  sslConfiguration.setPrivateKey(sslKey);
  sslConfiguration.setProtocol(QSsl::TlsV1SslV3);
  webSocket_.setSslConfiguration(sslConfiguration);

  connect(this, &QSfuSignaling::sendMessgeToSfu, this, &Controller::onSendMessgeToSfu);
  connect(&webSocket_, &QWebSocket::connected, this, &Controller::onConnectedSfu);
  connect(&webSocket_, &QWebSocket::disconnected, this, &Controller::onDisconnectedSfu);
  connect(&webSocket_, &QWebSocket::textMessageReceived, this, &Controller::onReceivedSfuMessage);
  connect(&webSocket_, &QWebSocket::sslErrors, this, &Controller::onSslErrors);
  connect(&webSocket_, &QWebSocket::stateChanged, this, &Controller::onStateChanged);
}

Controller::~Controller() {
  delete webrtcProxy_;
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
}

void Controller::onDisconnectedSfu()
{
  qDebug("[%s]", __func__);
}

void Controller::onReceivedSfuMessage(const QString& message)
{
  gotMsgFromSfu(message.toStdString());
}

void Controller::onStateChanged(QAbstractSocket::SocketState state)
{
  qDebug("[%s] state=%d", __func__, state);
}

void Controller::onSslErrors(const QList<QSslError> &errors)
{
  Q_UNUSED(errors);

  // WARNING: Never ignore SSL errors in production code.
  // The proper way to handle self-signed certificates is to add a custom root
  // to the CA store.

  webSocket_.ignoreSslErrors();
}

void Controller::onSendMessgeToSfu(const std::string& message)
{
  webSocket_.sendTextMessage(QString::fromStdString(message));
}

