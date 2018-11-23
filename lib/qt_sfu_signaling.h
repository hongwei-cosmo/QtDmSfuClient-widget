#ifndef QSFUSIGNALING_H
#define QSFUSIGNALING_H

#include <functional>
#include <memory>
#include <string>

#include "Client.h"

// NOTE: Must include QObject after Client.h
#include <QObject>
/**
 * @brief Qt Wrapper of class Client of dm-sfu-client-lib
 */
class QSfuSignaling :
    public QObject,
    public dm::Client::Transport
{
  Q_OBJECT
public:
  explicit QSfuSignaling(QObject *parent = nullptr);
  virtual ~QSfuSignaling() = default;

  void createRoom();
  void createAuditRoom(const std::string &recodingId);
  void destroyRoom();
  // As to join Room needs to get information from Webrtc, implement it in
  // Controller.
  virtual void joinRoom() = 0;
  void seekParticipant(uint64_t offset);
  void limitParticipant(uint16_t bitrate);
  void leaveRoom();
  void publishStream(const std::string &sdp, bool camera);
  void unpublishStream(bool camera);
  void lastN(int n);

  std::string getRoomId() const;
  std::string getRoomAccessPin() const;
  void setRoomId(std::string roomId_);
  void setRoomAccessPin(const std::string& pin);

  virtual void Log(const std::string &msg) = 0;

public Q_SLOTS:
  void onReceivedSfuMessage(const QString& message);

Q_SIGNALS:
  void sendSfuLog(const QString &log);
  void updateRemoteInfo();
  void participantJoinedEvent(const std::string &roomId, const std::string &clientId, const std::string &reason);
  void participantLeftEvent(const std::string &roomId, const std::string &clientId, const std::string &reason);
  void participantKickedEvent(const std::string &roomId, const std::string &reason);
  void activeSpeakerChangedEvent(const std::string &roomId, const std::string &clientId);
  void gotAnswerInfo(const std::string &sdp);

  void publishedStream(bool success);


protected:
  std::unique_ptr<dm::Client> sfu_;
  std::string roomId_;
  std::string roomAccessPin_ = "pin";
  SDPInfo::shared remoteSdpInfo_;
  std::function<bool (const std::string&)> callback_ = nullptr;

  // send will be overrided in Controller
  virtual void onmessage(const std::function<bool(const std::string &message)> &callback) final;
};

#endif // QSFUSIGNALING_H
