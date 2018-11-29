#include "ui/mainwindow.h"

#include <QDebug>
#include <QImage>
#include <QStandardPaths>

using namespace nlohmann;

Controller::Controller(QWidget *mainWindow) :
  mainWindow_(mainWindow)
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
    std::string certs = QStandardPaths::locate(QStandardPaths::HomeLocation,
                           "certs",
                           QStandardPaths::LocateDirectory).toStdString();

    try {
      // Removes support for undesired TLS versions
      ctx->set_options(asio::ssl::context::default_workarounds |
        asio::ssl::context::no_sslv2 |
        asio::ssl::context::no_sslv3 |
        asio::ssl::context::no_tlsv1 |
        asio::ssl::context::no_tlsv1_1 |
        asio::ssl::context::single_dh_use);

      //Try to setup certificate, do not use if not found
      qDebug("Trying to load certification from %s", certs.c_str());
      // Loads CA certs file used to verify peers (DM_SFU)
      ctx->load_verify_file(certs + "/ca_server.pem");
      // Sets lists of of ciphers offered and accepted
      SSL_CTX_set_cipher_list(ctx->native_handle(), "ECDHE-RSA-AES256-GCM-SHA384");
      // Loads client certificate and private key
      ctx->use_certificate_file(certs + "/client.pem", asio::ssl::context::file_format::pem);
      ctx->use_private_key_file(certs + "/client.key", asio::ssl::context::file_format::pem);
      // Activates verification mode and rejects unverified peers
//      ctx->set_verify_mode(asio::ssl::context::verify_peer
//                         | asio::ssl::context::verify_fail_if_no_peer_cert);
      // TODO: enable ssl verify
      ctx->set_verify_mode(asio::ssl::context::verify_none);

    } catch (std::exception &e) {
      qDebug("[EXCEPTION] ctx set_options:");
      qDebug("%s", e.what());
      qDebug("Peer verification is diabled");
    }
    return ctx;
  });

  pc_ = new rtc::RefCountedObject<DMPeerConnection>();
  if (!pc_->InitializePeerConnection(nullptr, 0, nullptr, nullptr, true)) {
    Error("Initialize Peer Conection Failed");
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
      Error("Failed to connect to sfu: " + std::to_string(ec.value()) + ec.message());
      return false;
    } else {
      Log("Connected to sfu");
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
    Error("[EXCEPTION] connect:");
    Error(e.what());
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
    Error("[EXCEPTION] close:");
    Error(e.what());
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
    Error("Controller::send() exception");
  }
}

void Controller::updateRemoteInfo()
{
  qDebug("[%s]", __func__);
  pc_->SetRemoteDescription("offer", remoteSdpInfo_->toString().c_str());
  pc_->CreateAnswer();
}

void Controller::participantJoined(const std::string &roomId,
                                     const std::string &clientId,
                                     const std::string &reason)
{
  qDebug("[%s] ClientId: %s", __func__, clientId.c_str());
}
void Controller::participantLeft(const std::string &roomId,
                                   const std::string &clientId,
                                   const std::string &reason)
{
  qDebug("[%s] ClientId: %s", __func__, clientId.c_str());
}
void Controller::participantKicked(const std::string &roomId,
                                     const std::string &reason)
{
  qDebug("[%s]", __func__);
}

void Controller::activeSpeakerChanged(const std::string &roomId,
                                        const std::string &clientId)
{
  qDebug("[%s] ClientId: %s", __func__, clientId.c_str());
}

void Controller::joinRoom()
{
  qDebug("[%s]", __func__);

  if (state == State::Joined) {
    Error("Already Joined");
    return;
  }

  pc_->RegisterOnRemoteI420FrameReady([&](const uint8_t* buffer,
                                     int width, int height) {
    QImage image(buffer, width, height, QImage::Format::Format_ARGB32);
    Q_EMIT dynamic_cast<MainWindow*>(mainWindow_)->getRemoteFrame()->paintImage(image);
  });

  pc_->RegisterOnLocalSdpReadytoSend([this](const char *type, const char *sdp) {
     // join
    Q_UNUSED(type);
    sfu_->join(roomId_, roomAccessPin_, SDPInfo::parse(sdp),
               [this](const dm::Participant::Joined &joined) {
      if (joined.error) {
        Error("Join Room Failed");
        return;
      } else {
        Log("Joined room");
      }

      remoteSdpInfo_ = joined.result->sdpInfo;
      for(auto stream : joined.result->streams) {
        remoteSdpInfo_->addStream(stream);
      }

      if (!pc_->SetRemoteDescription("answer", remoteSdpInfo_->toString().c_str())) {
        Error("Set Remote Description Failed");
        return;
      }

      // Default setting
      lastN(1);
      publishCamera();

      // TODO: Set profiles
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
        Error("Failed to publish stream: " + published.error->message);
        return;
      }

      if (!pc_->SetRemoteDescription("answer", remoteSdpInfo_->toString().c_str())) {
        Error("Set Remote Description Failed");
        return;
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
    QImage image(buffer, width, height, QImage::Format::Format_ARGB32);
    Q_EMIT dynamic_cast<MainWindow*>(mainWindow_)->getLocalFrame()->paintImage(image);
  });
  pc_->AddStreams(false);
  publish(true);
}

void Controller::publishDesktop()
{
  state = State::Desktop;
  publish(false);
}
