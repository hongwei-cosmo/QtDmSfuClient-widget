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
  enum class State {
    Disconnected  = 0b0000,
    Connected     = 0b0001,
    Joined        = 0b0011,
    Camera        = 0b0111,
    Desktop       = 0b1011,
  };

  explicit Controller(QObject *parent = nullptr);
  ~Controller();
  State getState() const;
  virtual void joinRoom() final;
  virtual void send(const std::string &message) final;
  void publishCamera();
  void publishDesktop();
  std::function<void(const std::string &log)> logger = nullptr;

  virtual void Log(const std::string &log) final;

public Q_SLOTS:
  void connectSfu(const std::string &sfuUrl, const std::string &clientId);
  void disconnectSfu();

private Q_SLOTS:
  void onConnectedSfu();
  void onDisconnectedSfu();
  void onStateChanged(QAbstractSocket::SocketState state);
  void onSslErrors(const QList<QSslError> &errors);

  void onStreamPublished();
  void onStreamUnpublished(const std::string &streamId);
  void onParticipantJoined(const std::string &roomId, const std::string &clientId, const std::string &reason);
  void onParticipantLeft(const std::string &roomId, const std::string &clientId, const std::string &reason);
  void onParticipantKicked(const std::string &roomId, const std::string &reason);
  void onActiveSpeakerChanged(const std::string &roomId, const std::string &clientId);
  void onPublishedStream(bool success);

  void onCreatedJoinRoomOfferSuccess(const QJsonObject &sdp);
  void onCreatedPublishCameraOfferSuccess(const QJsonObject &sdp);
  void onCreatedPublishDesktopOfferSuccess(const QJsonObject &sdp);
  void onGotICECandidate(const QJsonObject &candidate);
  void onAddedStream(MediaStreamProxy* stream);
  void onSetRemoteDescriptionSuccess();
  void onCreatedAnswerSuccess(const QJsonObject &sdp);

private:
  QWebRTCProxy *webrtcProxy_;
  PeerConnectionProxy *peerConnection_;
  QWebSocket webSocket_;
  State state = State::Disconnected;
};

#endif // CONTROLLER_H
