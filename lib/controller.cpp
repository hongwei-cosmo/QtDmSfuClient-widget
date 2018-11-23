#include "ui/mainwindow.h"

#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QLabel>

using namespace nlohmann;

Controller::Controller(QWidget *mainWindow, QObject *parent) :
  QSfuSignaling(parent), mainWindow_(mainWindow)
{
  // Set logging to be pretty verbose (everything except message payloads)
  client_.set_access_channels(websocketpp::log::alevel::all);
  client_.clear_access_channels(websocketpp::log::alevel::frame_payload);
  client_.set_error_channels(websocketpp::log::elevel::all);

  // Initialize ASIO
  client_.init_asio();

  // Register our tls hanlder
  client_.set_tls_init_handler([&](...) {
    // Create context
    auto ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::tlsv12_client);

    try {
      // Removes support for undesired TLS versions
      ctx->set_options(asio::ssl::context::default_workarounds |
        asio::ssl::context::no_sslv2 |
        asio::ssl::context::no_sslv3 |
        asio::ssl::context::no_tlsv1 |
        asio::ssl::context::no_tlsv1_1 |
        asio::ssl::context::single_dh_use);

      //Try to setup certificate, do not use if not found
      qDebug("Trying to load certificate...");
      // Loads CA certs file used to verify peers (DM_SFU)
      ctx->load_verify_file("C:/certs/ca_server.pem");
      // Sets lists of of ciphers offered and accepted
      SSL_CTX_set_cipher_list(ctx->native_handle(), "ECDHE-RSA-AES256-GCM-SHA384");
      // Loads client certificate and private key
      ctx->use_certificate_file("C:/certs/client.pem", asio::ssl::context::file_format::pem);
      ctx->use_private_key_file("C:/certs/client.key", asio::ssl::context::file_format::pem);
      // Activates verification mode and rejects unverified peers
//      ctx->set_verify_mode(asio::ssl::context::verify_peer
//                         | asio::ssl::context::verify_fail_if_no_peer_cert);
      // TODO: enable ssl verify
      ctx->set_verify_mode(asio::ssl::context::verify_none);

    } catch (std::exception &e) {
      qDebug("[EXCEPTION] ctx set_options:");
      qDebug(e.what());
      qDebug("Peer verification is diabled");
    }
    return ctx;
  });

  connect(this, &QSfuSignaling::updateRemoteInfo, this, &Controller::onUpdateRemoteInfo);
  connect(this, &QSfuSignaling::participantJoinedEvent, this, &Controller::onParticipantJoined);
  connect(this, &QSfuSignaling::participantLeftEvent, this, &Controller::onParticipantLeft);
  connect(this, &QSfuSignaling::participantKickedEvent, this, &Controller::onParticipantKicked);
  connect(this, &QSfuSignaling::activeSpeakerChangedEvent, this, &Controller::onActiveSpeakerChanged);

  pc_ = new rtc::RefCountedObject<DMPeerConnection>();
  if (!pc_->InitializePeerConnection(nullptr, 0, nullptr, nullptr, true)) {
    Log("Initialize Peer Conection Failed");
  }
}

Controller::~Controller() {
  //Stop client
  client_.stop();
  //Clean thread
  if (thread_.joinable()) {
    // Detach trhead
    thread_.detach();
  }

  pc_->DeletePeerConnection();
}

Controller::State Controller::getState() const
{
  return state;
}

bool Controller::connectSfu(const std::string& sfuUrl,
                            const std::string& clientId)
{
  qDebug("[%s] url=%s, id=%s", __func__, sfuUrl.c_str(), clientId.c_str());

  try {
    std::string ws = sfuUrl + "/?clientId=" + clientId;
    websocketpp::lib::error_code ec;

    // Get new connection
    connection_ = client_.get_connection(ws, ec);
    // Check error
    if (ec) {
      Log("get_connection failed. " + std::to_string(ec.value()) + ec.message());
      return false;
    }

    // Register our message handler
    connection_->set_message_handler([&](websocketpp::connection_hdl con,
         websocketpp::config::asio_client::message_type::ptr frame) {
      // Pass to the sfu client
      this->callback_(frame->get_payload());
    });

    // Set close hanlder
    connection_->set_close_handler([&](...) {
      // Call listener
      qDebug("Connection close handler");
      // Don't wait for connection close
      thread_.detach();
      // Remove connection
      connection_ = nullptr;
    });

    // Set failure handler
    connection_->set_fail_handler([&](...) {
      // Call listener
      qDebug("Connection fail handler");
      // Don't wait for connection close
      thread_.detach();
      // Remove connection
      connection_ = nullptr;
    });


    connection_->set_open_handler([=](websocketpp::connection_hdl con) {
      // Launch event
      qDebug("CONNECTED");
    });

    // Note that connect here only requests a connection. No network messages
    // are exchanged until the event loop starts running in the next line.
    client_.connect(connection_);

    // Async
    // NOTE: all websocketpp handlers run in the thread.
    // So can't call Log in this handlers, Log isn't thread safe.
    thread_ = std::thread([&]() {
      //Run client async
      client_.run();
    });
  }
  catch (websocketpp::exception const &e) {
    Log("[EXCEPTION] connect:");
    Log(e.what());
    return false;
  }

  state = State::Connected;
  return true;
}

bool Controller::disconnectSfu()
{
  qDebug("[%s]", __func__);
  if (state == State::Disconnected)
    return true;

  try {
    Log("Disconnecting ...");
    // Stop client
    client_.close(connection_,
                  websocketpp::close::status::normal,
                  std::string("disconnect"));
    connection_ = nullptr;
    Log("DISCONNECTED");
  } catch (websocketpp::exception const &e) {
    Log("[EXCEPTION] close:");
    Log(e.what());
    return false;
  }

  return true;
}

void Controller::send(const std::string &message)
{
  qDebug("[%s] msg=\"%s\"", __func__, message.c_str());
  try {
    // Send it
    connection_->send(message);
  } catch (...) {
    Log("[ERROR]Controller::send() exception");
  }
}

void Controller::onUpdateRemoteInfo()
{
  qDebug("[%s]", __func__);
  pc_->SetRemoteDescription("offer", remoteSdpInfo_->toString().c_str());
  pc_->CreateAnswer();
}

void Controller::onParticipantJoined(const std::string &roomId,
                                     const std::string &clientId,
                                     const std::string &reason)
{
  qDebug("[%s]", __func__);
}
void Controller::onParticipantLeft(const std::string &roomId,
                                   const std::string &clientId,
                                   const std::string &reason)
{
  qDebug("[%s]", __func__);
}
void Controller::onParticipantKicked(const std::string &roomId,
                                     const std::string &reason)
{
  qDebug("[%s]", __func__);
}
void Controller::onActiveSpeakerChanged(const std::string &roomId,
                                        const std::string &clientId)
{
  qDebug("[%s]", __func__);

}

void Controller::onPublishedStream(bool success)
{
  qDebug("[%s]", __func__);
}

void Controller::joinRoom()
{
  qDebug("[%s]", __func__);

  if (state == State::Joined) {
    Log("Error: Already joined");
    return;
  }

  pc_->RegisterOnRemoteI420FrameReady([&](const uint8_t* buffer,
                                     int width, int height) {
    qDebug("Remote Image %d, %d", width, height);
    QImage image(buffer, width, height, QImage::Format::Format_ARGB32);
    dynamic_cast<MainWindow*>(mainWindow_)->getRemoteFrame()->drawImage(image);
  });

  pc_->RegisterOnLocalSdpReadytoSend([this](const char *type, const char *sdp) {
     // join
    sfu_->join(roomId_, roomAccessPin_, SDPInfo::parse(sdp), [this](const dm::Participant::Joined &joined) {
      if (joined.error) {
        Log("Error: Join Room Failed");
        return;
      }

      remoteSdpInfo_ = joined.result->sdpInfo;
      for(auto stream : joined.result->streams) {
        remoteSdpInfo_->addStream(stream);
      }

      if (!pc_->SetRemoteDescription("answer", remoteSdpInfo_->toString().c_str())) {
        Log("Error: Set Remote Description Failed");
        return;
      }

      // TODO: Set profiles
//      std::vector<dm::VideoProfile> profiles = {
//        {"camera"	, dm::LayerTraversalAlgorithm::ZigZagSpatialTemporal},
//        {"screenshare"	, dm::LayerTraversalAlgorithm::SpatialTemporal}
//      };
    });

  });
  pc_->CreateOffer();
}

void Controller::publish(bool camera)
{
  pc_->RegisterOnLocalSdpReadytoSend([&](const char *type, const char *sdp) {
    std::string tag = camera ? "camera" : "desktop";
    auto sdpInfo = SDPInfo::parse(sdp);
    auto streamInfo = sdpInfo->getStream(tag);
    sfu_->publish(roomId_, camera ? dm::StreamKind::Camera : dm::StreamKind::Desktop,
                  tag, streamInfo, [=](const dm::Stream::Published &published) {
      if (published.error) {
        Log("PublishStream: Failed. Error: " + published.error->message);
      }
    });
  });
  pc_->CreateOffer();
}

void Controller::publishCamera()
{
  qDebug("[%s]", __func__);
  pc_->RegisterOnLocalI420FrameReady([&](const uint8_t* buffer,
                                     int width, int height) {
    qDebug("Image %d, %d", width, height);
    QImage image(buffer, width, height, QImage::Format::Format_ARGB32);
    dynamic_cast<MainWindow*>(mainWindow_)->getLocalFrame()->drawImage(image);

  });
  pc_->AddStreams(false);
  publish(true);
}

void Controller::publishDesktop()
{
  state = State::Desktop;
  publish(false);
}

// Note: this function can only be called in functions of Qbject class
void Controller::Log(const std::string &log)
{
  qDebug("[Log: %s]", log.c_str());
//  if (logger) logger(log);
}

void Controller::onCreatedJoinRoomOfferSuccess(const QJsonObject &sdp)
{
  qDebug("[%s]", __func__);
}

void Controller::onCreatedPublishCameraOfferSuccess(const QJsonObject &sdp)
{
  qDebug("[%s]", __func__);
}

void Controller::onCreatedPublishDesktopOfferSuccess(const QJsonObject &sdp)
{
  qDebug("[%s]", __func__);
}

void Controller::onGotICECandidate(const QJsonObject &candidate)
{
  qDebug("[%s]", __func__);
  Q_UNUSED(candidate);
}
void Controller::onSetRemoteDescriptionSuccess()
{
  qDebug("[%s]", __func__);
}
void Controller::onCreatedAnswerSuccess(const QJsonObject &sdp)
{
  qDebug("[%s]", __func__);
  Q_UNUSED(sdp);
}
