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
  bool connectedSfu() const;
  void createOffer();

Q_SIGNALS:

public Q_SLOTS:
  void connectSfu(const std::string &sfuUrl, const std::string &clientId);
  void disconnectSfu();

private Q_SLOTS:
  void onConnectedSfu();
  void onDisconnectedSfu();
  void onStateChanged(QAbstractSocket::SocketState state);
  void onSslErrors(const QList<QSslError> &errors);

  void onSendMessgeToSfu(const std::string &message);
  void onCommandFinished(const std::string &cmd, const std::string &result);

private:
  QWebRTCProxy *webrtcProxy_;
  QWebSocket webSocket_;
  bool connectedSfu_ = false;
};

#endif // CONTROLLER_H
