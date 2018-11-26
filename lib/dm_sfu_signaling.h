#ifndef DMSFUSIGNALING_H
#define DMSFUSIGNALING_H

#include <functional>
#include <memory>
#include <string>

#include "Client.h"

/**
 * @brief Qt Wrapper of class Client of dm-sfu-client-lib
 */
class DMSfuSignaling :
    public dm::Client::Transport
{
public:
  DMSfuSignaling();
  virtual ~DMSfuSignaling() = default;

  void createRoom();
  void createAuditRoom(const std::string &recodingId);
  void destroyRoom();
  // As to join Room needs to get information from Webrtc, implement it in
  // Controller.
  virtual void joinRoom() = 0;
  void seekParticipant(uint64_t offset);
  void limitParticipant(uint16_t bitrate);
  void leaveRoom();
  void unpublishStream(bool camera);
  void lastN(int n);

  std::string getRoomId() const;
  std::string getRoomAccessPin() const;
  void setRoomId(std::string roomId_);
  void setRoomAccessPin(const std::string& pin);

  std::function<void(const std::string &log)> logger = nullptr;

protected:
  std::unique_ptr<dm::Client> sfu_;
  std::string roomId_;
  std::string roomAccessPin_ = "pin";
  SDPInfo::shared remoteSdpInfo_;
  std::function<bool (const std::string&)> callback_ = nullptr;

  void Log(const std::string &msg);
  void Error(const std::string &msg);

  virtual void updateRemoteInfo() = 0;
  virtual void participantJoined(const std::string &roomId, const std::string &clientId, const std::string &reason) = 0;
  virtual void participantLeft(const std::string &roomId, const std::string &clientId, const std::string &reason) = 0;
  virtual void participantKicked(const std::string &roomId, const std::string &reason) = 0;
  virtual void activeSpeakerChanged(const std::string &roomId, const std::string &clientId) = 0;

  virtual void onmessage(const std::function<bool(const std::string &message)> &callback) final;
  // send will be overrided in Controller
};

#endif // DMSFUSIGNALING_H
