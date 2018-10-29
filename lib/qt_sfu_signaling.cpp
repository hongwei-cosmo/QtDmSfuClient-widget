#include "qt_sfu_signaling.h"
#include <QDebug>

#define LOG Q_EMIT sendSfuLog

QSfuSignaling::QSfuSignaling(QObject *parent) : QObject(parent)
{
  qDebug("[%s]", __func__);
  sfu_ = std::make_unique<dm::Client>(*this);
  // Set event handlers for sfu_
  sfu_->on<dm::Stream::Event::Published>([=](dm::Stream::Event::Published &r) {
    LOG("Sfu event: Stream Published");
    this->sdpInfo_->addStream(r.streamInfo);
    Q_EMIT streamPublished();
  })
  .on<dm::Stream::Event::Unpublished>([=](dm::Stream::Event::Unpublished &r) {
    LOG("Sfu event: Stream unpublished");
    sdpInfo_->removeStream(r.streamId);
    Q_EMIT streamUnpublished(r.streamId);
  })
  .on<dm::Participant::Event::Joined>([=](dm::Participant::Event::Joined &r) {
    LOG("Sfu event: Participant Joined");
    Q_EMIT participantJoined(r.roomId, r.clientId, r.reason);
  })
  .on<dm::Participant::Event::Left>([=](dm::Participant::Event::Left &r) {
    LOG("Sfu event: Participant Left");
    Q_EMIT participantLeft(r.roomId, r.clientId, r.reason);
  })
  .on<dm::Participant::Event::Kicked>([=](dm::Participant::Event::Kicked &r) {
    LOG("Sfu event: Participant Kicked");
    Q_EMIT participantKicked(r.roomId, r.reason);
  })
  .on<dm::Participant::Event::ActiveSpeakerChanded>(
    [=](dm::Participant::Event::ActiveSpeakerChanded &r) {
      LOG("Sfu event: Active Speaker Changed");
      Q_EMIT activeSpeakerChanged(r.roomId, r.clientId);
  });
}

void QSfuSignaling::createRoom()
{
  qDebug("[%s]", __func__);
  sfu_->createRoom(roomAccessPin_,
                        [this](const dm::Room::Created &r) {
    LOG("Create Room " + r.toString());
    if (!r.error) {
      roomId_ = r.result->id;
      LOG("\tid=" + roomId_);
    }
  });
}

void QSfuSignaling::createAuditRoom(const std::string& recodingId)
{
  qDebug("[%s]", __func__);
  sfu_->createAuditRoom(roomAccessPin_, recodingId,
                              [=](const dm::Room::Created &r) {
    if (!r.error) {
      roomId_ = r.result->id;
    }
    LOG("Create Audit Room " + r.toString());
  });
}

void QSfuSignaling::destroyRoom()
{
  qDebug("[%s]", __func__);
  sfu_->destroyRoom(roomId_, [this](const dm::Room::Destroyed &r) {
    LOG("Destroy Room " + r.toString());
  });
}

void QSfuSignaling::joinRoom(const std::string& sdp)
{
  qDebug("[%s]", __func__);
  auto offerInfo = SDPInfo::parse(sdp);
  sfu_->join(roomId_, roomAccessPin_, offerInfo,
                  [this](const dm::Participant::Joined &r) {
    if (!r.error) {
      sdpInfo_ = r.result->sdpInfo;
      for (auto stream : r.result->streams) {
        sdpInfo_->addStream(stream);
      }

      Q_EMIT gotAnswerInfo(sdpInfo_->toString());

      // TODO: crash on mac
//      std::vector<dm::VideoProfile> profiles = {
//          {"camera", dm::LayerTraversalAlgorithm::ZigZagSpatialTemporal},
//          {"screenshare", dm::LayerTraversalAlgorithm::SpatialTemporal},
//      };
//      sfu_->setProfiles(roomId_, profiles, [](...){
//          qDebug("Profiles set");
//      });
    }
    LOG("Join Room " + r.toString());
  });
}

void QSfuSignaling::seekParticipant(uint64_t offset)
{
  qDebug("[%s]", __func__);
  sfu_->seek(roomId_, offset, [this](const dm::Participant::Seeked &r) {
    LOG("Seek Participant " + r.toString());
  });
}

void QSfuSignaling::limitParticipant(uint16_t bitrate)
{
  qDebug("[%s]", __func__);
  sfu_->limit(roomId_, bitrate, [this](const dm::Participant::Limited &r) {
    LOG("Limit Participant " + r.toString());
  });

}

void QSfuSignaling::leaveRoom()
{
  qDebug("[%s]", __func__);
  sfu_->leave(roomId_, [this](const dm::Participant::Left &r) {
    LOG("Leave Room " + r.toString());
  });
}

void QSfuSignaling::cameraStream()
{

}

void QSfuSignaling::desktopStream()
{

}

void QSfuSignaling::lastN(int n)
{
  switch (n) {
  case 0:
    sfu_->lastN(roomId_, n, {}, 0, 1, {}, [this](const dm::Participant::LastN &r) {
      LOG("Last None " + r.toString());
    });
    break;
  case 1:
    sfu_->lastN(roomId_, n, {1}, 1, 1, {1}, [this](const dm::Participant::LastN &r) {
      LOG("Last 1 " + r.toString());
    });
    break;
  case 2:
    sfu_->lastN(roomId_, n, {4, 1}, 1, 1, {2}, [this](const dm::Participant::LastN &r) {
      LOG("Last 2 " + r.toString());
    });
    break;
  case 3:
    sfu_->lastN(roomId_, n, {4, 1, 1}, 3, 3, {1, 1, 1}, [this](const dm::Participant::LastN &r) {
      LOG("Last 3 " + r.toString());
    });
    break;
  case 4:
    sfu_->lastN(roomId_, n, {3, 1, 1, 1}, 1, 4, {1}, [this](const dm::Participant::LastN &r) {
      LOG("Last 4 " + r.toString());
    });
    break;
  case -1:
  default:
    sfu_->lastN(roomId_, -1, {}, -1, 1, {}, [this](const dm::Participant::LastN &r) {
      LOG("Last All " + r.toString());
    });
    break;
  }
}

void QSfuSignaling::setRoomId(std::string roomId)
{
  qDebug("[%s]", __func__);
  roomId_ = roomId;
}

std::string QSfuSignaling::getRoomId() const
{
  qDebug("[%s]", __func__);
  return roomId_;
}

void QSfuSignaling::setRoomAccessPin(std::string pin)
{
  qDebug("[%s]", __func__);
  roomAccessPin_ = pin;
}

std::string QSfuSignaling::getAnswerSdp() const
{
  qDebug("[%s]", __func__);
  return sdpInfo_->toString();
}

std::string QSfuSignaling::getRoomAccessPin() const
{
  return roomAccessPin_;
}

void QSfuSignaling::onReceivedSfuMessage(const QString& message)
{
  qDebug("[%s]", __func__);
  callback_(message.toStdString());
}

void QSfuSignaling::send(const std::string& message)
{
  qDebug("[%s]", __func__);
  Q_EMIT sendMessgeToSfu(message);
}

void QSfuSignaling::onmessage(const std::function<bool (const std::string&)> &callback)
{
  qDebug("[%s]", __func__);
  this->callback_ = callback;
}
