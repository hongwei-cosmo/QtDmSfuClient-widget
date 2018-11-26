#ifndef CONTROLLER_H
#define CONTROLLER_H

// Websocketpp
#include "websocketpp/client.hpp"
#include "websocketpp/config/asio_client.hpp"
#include "qt_sfu_signaling.h"
#include "dm_peer_connection.h"

#include <QObject>
#include <QJsonObject>

class Controller final : public QSfuSignaling
{
public:
  enum class State {
    Disconnected  = 0b0000,
    Connected     = 0b0001,
    Joined        = 0b0011,
    Camera        = 0b0111,
    Desktop       = 0b1011,
  };

  explicit Controller(QWidget *mainWindow);
  ~Controller();
  State getState() const;
  bool connectSfu(const std::string &sfuUrl, const std::string &clientId);
  bool disconnectSfu();
  virtual void joinRoom() override;
  virtual void send(const std::string &message) override;
  void publishCamera();
  void publishDesktop();

  void updateRemoteInfo() override;
  void participantJoined(const std::string &roomId, const std::string &clientId, const std::string &reason) override;
  void participantLeft(const std::string &roomId, const std::string &clientId, const std::string &reason) override;
  void participantKicked(const std::string &roomId, const std::string &reason) override;
  void activeSpeakerChanged(const std::string &roomId, const std::string &clientId) override;

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
