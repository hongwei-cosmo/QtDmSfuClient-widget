#include "qt_sfu_signaling.h"
#include <QDebug>

QSfuSignaling::QSfuSignaling(QObject *parent) : QObject(parent)
{
  qDebug("[%s]", __func__);
  sfu_ = std::make_unique<dm::Client>(*this);
  // Set event handlers for sfu_
  sfu_->on<dm::Stream::Event::Published>([=](dm::Stream::Event::Published &r) {
    Log("Sfu event: Stream Published");
    this->sdpInfo_->addStream(r.streamInfo);
    Q_EMIT streamPublishedEvent();
  })
  .on<dm::Stream::Event::Unpublished>([=](dm::Stream::Event::Unpublished &r) {
    Log("Sfu event: Stream unpublished");
    sdpInfo_->removeStream(r.streamId);
    Q_EMIT streamUnpublishedEvent(r.streamId);
  })
  .on<dm::Participant::Event::Joined>([=](dm::Participant::Event::Joined &r) {
    Log("Sfu event: New Participant Joined");
    Q_EMIT participantJoinedEvent(r.roomId, r.clientId, r.reason);
  })
  .on<dm::Participant::Event::Left>([=](dm::Participant::Event::Left &r) {
    Log("Sfu event: Participant Left");
    Q_EMIT participantLeftEvent(r.roomId, r.clientId, r.reason);
  })
  .on<dm::Participant::Event::Kicked>([=](dm::Participant::Event::Kicked &r) {
    Log("Sfu event: Participant Kicked");
    Q_EMIT participantKickedEvent(r.roomId, r.reason);
  })
  .on<dm::Participant::Event::ActiveSpeakerChanded>(
    [=](dm::Participant::Event::ActiveSpeakerChanded &r) {
      Log("Sfu event: Active Speaker Changed");
      Q_EMIT activeSpeakerChangedEvent(r.roomId, r.clientId);
  });
}

void QSfuSignaling::createRoom()
{
  qDebug("[%s]", __func__);
  sfu_->createRoom(roomAccessPin_,
                        [this](const dm::Room::Created &r) {
    Log("Create Room " + r.toString());
    if (!r.error) {
      roomId_ = r.result->id;
      Log("\tid=" + roomId_);
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
    Log("Create Audit Room " + r.toString());
  });
}

void QSfuSignaling::destroyRoom()
{
  qDebug("[%s]", __func__);
  sfu_->destroyRoom(roomId_, [this](const dm::Room::Destroyed &r) {
    Log("Destroy Room " + r.toString());
  });
}

void QSfuSignaling::seekParticipant(uint64_t offset)
{
  qDebug("[%s]", __func__);
  sfu_->seek(roomId_, offset, [this](const dm::Participant::Seeked &r) {
    Log("Seek Participant " + r.toString());
  });
}

void QSfuSignaling::limitParticipant(uint16_t bitrate)
{
  qDebug("[%s]", __func__);
  sfu_->limit(roomId_, bitrate, [this](const dm::Participant::Limited &r) {
    Log("Limit Participant " + r.toString());
  });

}

void QSfuSignaling::leaveRoom()
{
  qDebug("[%s]", __func__);
  sfu_->leave(roomId_, [this](const dm::Participant::Left &r) {
    Log("Leave Room " + r.toString());
  });
}

void QSfuSignaling::publishStream(const std::string &sdp, bool camera)
{
  qDebug("[%s]", __func__);
  std::string tag = camera ? "camera" : "desktop";
  auto sdpInfo = SDPInfo::parse(sdp);
  auto streamInfo = sdpInfo->getStream(tag);
  sfu_->publish(roomId_, camera ? dm::StreamKind::Camera : dm::StreamKind::Desktop,
                tag, streamInfo, [=](const dm::Stream::Published &published) {
    if (published.error) {
      Log("PublishStream: Failed. Error: " + published.error->message);
    }
    Q_EMIT publishedStream((published.error?false:true));
  });
}

void QSfuSignaling::unpublishStream(bool camera)
{
  qDebug("[%s]", __func__);
  std::string tag = camera ? "camera" : "desktop";
  sfu_->unpublish(roomId_, tag, [=](const dm::Stream::Unpublished &unpublished) {

  });
}

void QSfuSignaling::lastN(int n)
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

void QSfuSignaling::onmessage(const std::function<bool (const std::string&)> &callback)
{
  qDebug("[%s]", __func__);
  this->callback_ = callback;
}
