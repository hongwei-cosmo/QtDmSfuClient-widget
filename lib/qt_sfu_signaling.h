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

  std::string getRoomId() const;
  std::string getAnswerSdp() const;
  std::string getRoomAccessPin() const;

public Q_SLOTS:
  void createRoom();
  void createAuditRoom(const std::string &recodingId);
  void destroyRoom();

  void onReceivedSfuMessage(const QString& message);

  void joinRoom(const std::string &sdp);
  void seekParticipant(uint64_t offset);
  void limitParticipant(uint16_t bitrate);
  void leaveRoom();

  void cameraStream();
  void desktopStream();

  void lastN(int n);

  void setRoomId(std::string roomId_);
  void setRoomAccessPin(std::string pin);

Q_SIGNALS:
  /**
   * @brief Signal used to get the status of the command
   * @param cmdId Command ID
   * @param result "OK" if command is successful. Or it is the error message
   */
  void commandFinished(const std::string &cmd, const std::string &result);
  void streamPublished();
  void streamUnpublished(const std::string &streamId);
  void participantJoined(const std::string &roomId, const std::string &clientId, const std::string &reason);
  void participantLeft(const std::string &roomId, const std::string &clientId, const std::string &reason);
  void participantKicked(const std::string &roomId, const std::string &reason);
  void activeSpeakerChanged(const std::string &roomId, const std::string &clientId);
  void sendMessgeToSfu(const std::string &message);
  void gotAnswerInfo(const std::string &sdp);


private:
  std::unique_ptr<dm::Client> sfu_;
  std::string roomId_;
  std::string roomAccessPin_ = "pin";
  SDPInfo::shared sdpInfo_;
  std::function<bool (const std::string&)> callback_ = nullptr;

  virtual void send(const std::string &message) override;
  virtual void onmessage(const std::function<bool(const std::string &message)> &callback) override;
};

#endif // QSFUSIGNALING_H
