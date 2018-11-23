#ifndef CONTROLLER_H
#define CONTROLLER_H

// Websocketpp
#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_client.hpp"
#include "qt_sfu_signaling.h"
#include "dm_peer_connection.h"

#include <QObject>
#include <QJsonObject>

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

  explicit Controller(QWidget *mainWindow, QObject *parent = nullptr);
  ~Controller();
  State getState() const;
  bool connectSfu(const std::string &sfuUrl, const std::string &clientId);
  bool disconnectSfu();
  virtual void joinRoom() final;
  virtual void send(const std::string &message) final;
  void publishCamera();
  void publishDesktop();
  std::function<void(const std::string &log)> logger = nullptr;

  virtual void Log(const std::string &log) final;
  void Error(const std::string &err) { Log("[Error] " + err); }

private Q_SLOTS:
  void onUpdateRemoteInfo();
  void onParticipantJoined(const std::string &roomId, const std::string &clientId, const std::string &reason);
  void onParticipantLeft(const std::string &roomId, const std::string &clientId, const std::string &reason);
  void onParticipantKicked(const std::string &roomId, const std::string &reason);
  void onActiveSpeakerChanged(const std::string &roomId, const std::string &clientId);
  void onPublishedStream(bool success);

  void onCreatedJoinRoomOfferSuccess(const QJsonObject &sdp);
  void onCreatedPublishCameraOfferSuccess(const QJsonObject &sdp);
  void onCreatedPublishDesktopOfferSuccess(const QJsonObject &sdp);
  void onGotICECandidate(const QJsonObject &candidate);
  void onSetRemoteDescriptionSuccess();
  void onCreatedAnswerSuccess(const QJsonObject &sdp);

private:
  websocketpp::client<websocketpp::config::asio_tls_client> client_;
  websocketpp::client<websocketpp::config::asio_tls_client>::connection_ptr connection_;
  std::thread thread_;
  rtc::scoped_refptr<DMPeerConnection> pc_;

  QWidget *mainWindow_ = nullptr;

  State state = State::Disconnected;

  void publish(bool camera);
};

#endif // CONTROLLER_H
