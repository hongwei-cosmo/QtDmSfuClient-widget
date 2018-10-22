#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "qt_sfu_signaling.h"

#include <QObject>
#include <QWebSocket>

class QWebRTCProxy;

class Controller : public QSfuSignaling
{
  Q_OBJECT
public:
  explicit Controller(QObject *parent = nullptr);
  ~Controller();

Q_SIGNALS:

public Q_SLOTS:
  void connectSfu(const std::string &sfuUrl, const std::string &clientId);
  void disconnectSfu();

private Q_SLOTS:
  void onConnectedSfu();
  void onDisconnectedSfu();
  void onReceivedSfuMessage(const QString &message);
  void onStateChanged(QAbstractSocket::SocketState state);
  void onSslErrors(const QList<QSslError> &errors);
  void onSendMessgeToSfu(const std::string &message);


private:
  QWebRTCProxy *webrtcProxy_;
  QWebSocket webSocket_;
};

#endif // CONTROLLER_H
