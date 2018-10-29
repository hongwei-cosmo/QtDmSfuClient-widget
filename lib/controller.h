#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "qt_sfu_signaling.h"

#include <QObject>
#include <QWebSocket>
#include <QJsonObject>

class QWebRTCProxy;
class MediaStreamProxy;
class PeerConnectionProxy;

class Controller : public QSfuSignaling
{
  Q_OBJECT
public:
  explicit Controller(QObject *parent = nullptr);
  ~Controller();
  bool connectedSfu() const;
  void createOffer();

Q_SIGNALS:
  void showLog(const QString &log);

public Q_SLOTS:
  void connectSfu(const std::string &sfuUrl, const std::string &clientId);
  void disconnectSfu();

private Q_SLOTS:
  void onConnectedSfu();
  void onDisconnectedSfu();
  void onStateChanged(QAbstractSocket::SocketState state);
  void onSslErrors(const QList<QSslError> &errors);

  void onSendMessgeToSfu(const std::string &message);
  void onGotSfuLog(const std::string &log);
  void onStreamPublished();
  void onStreamUnpublished(const std::string &streamId);
  void onParticipantJoined(const std::string &roomId, const std::string &clientId, const std::string &reason);
  void onParticipantLeft(const std::string &roomId, const std::string &clientId, const std::string &reason);
  void onParticipantKicked(const std::string &roomId, const std::string &reason);
  void onActiveSpeakerChanged(const std::string &roomId, const std::string &clientId);

  void onCreatedOfferSuccess(const QJsonObject &sdp);
  void onGotICECandidate(const QJsonObject &candidate);
  void onAddedStream(MediaStreamProxy* stream);
  void onSetRemoteDescriptionSuccess();
  void onCreatedAnswerSuccess(const QJsonObject &sdp);
  void onGotAnswerInfo(const std::string &sdp);


private:
  QWebRTCProxy *webrtcProxy_;
  PeerConnectionProxy *peerConnection_;
  QWebSocket webSocket_;
  bool connectedSfu_ = false;
  bool joinedRoom_ = false;
};

#endif // CONTROLLER_H
