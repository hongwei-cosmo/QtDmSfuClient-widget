#include "dm_sfu_signaling.h"
#include <QDebug>

DMSfuSignaling::DMSfuSignaling()
{
  qDebug("[%s]", __func__);
  sfu_ = std::make_unique<dm::Client>(*this);
  // Set event handlers for sfu_
  sfu_->on<dm::Stream::Event::Published>([=](dm::Stream::Event::Published &r) {
    Log("Sfu event: Add Steam ID = " + r.streamInfo->getId());
    remoteSdpInfo_->addStream(r.streamInfo);
    updateRemoteInfo();
  })
  .on<dm::Stream::Event::Unpublished>([=](dm::Stream::Event::Unpublished &r) {
    Log("Sfu event: Remove Stream ID = " + r.streamId);
    remoteSdpInfo_->removeStream(r.streamId);
    updateRemoteInfo();
  })
  .on<dm::Participant::Event::Joined>([=](dm::Participant::Event::Joined &r) {
    Log("Sfu event: New Participant Joined. ID: " + r.clientId);
    participantJoined(r.roomId, r.clientId, r.reason);
  })
  .on<dm::Participant::Event::Left>([=](dm::Participant::Event::Left &r) {
    Log("Sfu event: Participant Left. ID: " + r.clientId);
    participantLeft(r.roomId, r.clientId, r.reason);
  })
  .on<dm::Participant::Event::Kicked>([=](dm::Participant::Event::Kicked &r) {
    Log("Sfu event: Participant Kicked");
    participantKicked(r.roomId, r.reason);
  })
  .on<dm::Participant::Event::ActiveSpeakerChanded>(
    [=](dm::Participant::Event::ActiveSpeakerChanded &r) {
      Log("Sfu event: Active Speaker Changed to " + r.clientId);
      activeSpeakerChanged(r.roomId, r.clientId);
  });
}

void DMSfuSignaling::createRoom()
{
  qDebug("[%s]", __func__);
  sfu_->createRoom(roomAccessPin_,
                        [this](const dm::Room::Created &r) {
    if (!r.error) {
      roomId_ = r.result->id;
      Log("Room created, id: " + roomId_);
    } else {
      Error("Failed to create room");
    }
  });
}

void DMSfuSignaling::createAuditRoom(const std::string& recodingId)
{
  qDebug("[%s]", __func__);
  sfu_->createAuditRoom(roomAccessPin_, recodingId,
                              [=](const dm::Room::Created &r) {
    if (!r.error) {
      roomId_ = r.result->id;
    }
    Log("Create Audit Room " + r.toString());
  });
}

void DMSfuSignaling::destroyRoom()
{
  qDebug("[%s]", __func__);
  sfu_->destroyRoom(roomId_, [this](const dm::Room::Destroyed &r) {
    Log("Destroy Room " + r.toString());
  });
}

void DMSfuSignaling::seekParticipant(uint64_t offset)
{
  qDebug("[%s]", __func__);
  sfu_->seek(roomId_, offset, [this](const dm::Participant::Seeked &r) {
    Log("Seek Participant " + r.toString());
  });
}

void DMSfuSignaling::limitParticipant(uint16_t bitrate)
{
  qDebug("[%s]", __func__);
  sfu_->limit(roomId_, bitrate, [this](const dm::Participant::Limited &r) {
    Log("Limit Participant " + r.toString());
  });

}

void DMSfuSignaling::leaveRoom()
{
  qDebug("[%s]", __func__);
  sfu_->leave(roomId_, [this](const dm::Participant::Left &r) {
    Log("Leave Room " + r.toString());
  });
}

void DMSfuSignaling::unpublishStream(bool camera)
{
  qDebug("[%s]", __func__);
  std::string tag = camera ? "camera" : "desktop";
  sfu_->unpublish(roomId_, tag, [=](const dm::Stream::Unpublished &unpublished) {

  });
}

void DMSfuSignaling::lastN(int n)
{
  switch (n) {
  case 0:
    sfu_->lastN(roomId_, n, {}, 0, 1, {}, [this](const dm::Participant::LastN &r) {
      Log("Last None " + r.toString());
    });
    break;
  case 1:
    sfu_->lastN(roomId_, n, {1}, 1, 1, {1}, [this](const dm::Participant::LastN &r) {
      Log("Last 1 " + r.toString());
    });
    break;
  case 2:
    sfu_->lastN(roomId_, n, {4, 1}, 1, 1, {2}, [this](const dm::Participant::LastN &r) {
      Log("Last 2 " + r.toString());
    });
    break;
  case 3:
    sfu_->lastN(roomId_, n, {4, 1, 1}, 3, 3, {1, 1, 1}, [this](const dm::Participant::LastN &r) {
      Log("Last 3 " + r.toString());
    });
    break;
  case 4:
    sfu_->lastN(roomId_, n, {3, 1, 1, 1}, 1, 4, {1}, [this](const dm::Participant::LastN &r) {
      Log("Last 4 " + r.toString());
    });
    break;
  case -1:
  default:
    sfu_->lastN(roomId_, -1, {}, -1, 1, {}, [this](const dm::Participant::LastN &r) {
      Log("Last All " + r.toString());
    });
    break;
  }
}

void DMSfuSignaling::setRoomId(std::string roomId)
{
  qDebug("[%s]", __func__);
  roomId_ = roomId;
}

std::string DMSfuSignaling::getRoomId() const
{
  qDebug("[%s]", __func__);
  return roomId_;
}

void DMSfuSignaling::setRoomAccessPin(const std::string& pin)
{
  qDebug("[%s]", __func__);
  roomAccessPin_ = pin;
}

std::string DMSfuSignaling::getRoomAccessPin() const
{
  return roomAccessPin_;
}

void DMSfuSignaling::onmessage(const std::function<bool (const std::string&)> &callback)
{
  qDebug("[%s]", __func__);
  this->callback_ = callback;
}

void DMSfuSignaling::Log(const std::string &log)
{
  qDebug("[Log: %s]", log.c_str());
//  if (logger) logger(log);
}

void DMSfuSignaling::Error(const std::string &err)
{
  qCritical("[Error: %s]", err.c_str());
//  if (logger) logger(err);
}
