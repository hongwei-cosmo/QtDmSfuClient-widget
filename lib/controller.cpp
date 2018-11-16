#include "controller.h"

#include <QDebug>

using namespace nlohmann;

Controller::Controller(QObject *parent) :
  QSfuSignaling(parent)
{
  // Set logging to be pretty verbose (everything except message payloads)
  client.set_access_channels(websocketpp::log::alevel::all);
  client.clear_access_channels(websocketpp::log::alevel::frame_payload);
  client.set_error_channels(websocketpp::log::elevel::all);

  // Initialize ASIO
  client.init_asio();

  // Register our tls hanlder
  client.set_tls_init_handler([&](...) {
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
      Log("Trying to load certificate...");
      // Loads CA certs file used to verify peers (DM_SFU)
      ctx->load_verify_file("C:/certs/ca_server.pem");
      // Sets lists of of ciphers offered and accepted
      SSL_CTX_set_cipher_list(ctx->native_handle(), "ECDHE-RSA-AES256-GCM-SHA384");
      // Loads client certificate and private key
      ctx->use_certificate_file("C:/certs/client.pem", asio::ssl::context::file_format::pem);
      ctx->use_private_key_file("C:/certs/client.key", asio::ssl::context::file_format::pem);
      // Activates verification mode and rejects unverified peers
      ctx->set_verify_mode(asio::ssl::context::verify_peer
                         | asio::ssl::context::verify_fail_if_no_peer_cert);

    } catch (std::exception &e) {
      Log("[EXCEPTION] ctx set_options:");
      Log(e.what());
      Log("Peer verification is diabled");
    }
    return ctx;
  });

  connect(this, &QSfuSignaling::streamPublishedEvent, this, &Controller::onStreamPublished);
  connect(this, &QSfuSignaling::streamUnpublishedEvent, this, &Controller::onStreamUnpublished);
  connect(this, &QSfuSignaling::participantJoinedEvent, this, &Controller::onParticipantJoined);
  connect(this, &QSfuSignaling::participantLeftEvent, this, &Controller::onParticipantLeft);
  connect(this, &QSfuSignaling::participantKickedEvent, this, &Controller::onParticipantKicked);
  connect(this, &QSfuSignaling::activeSpeakerChangedEvent, this, &Controller::onActiveSpeakerChanged);
}

Controller::~Controller() {
  //Stop client
  client.stop();
  //Clean thread
  if (thread.joinable()) {
    // Detach trhead
    thread.detach();
  }
}

Controller::State Controller::getState() const
{
  return state;
}

bool Controller::connectSfu(const std::string& sfuUrl, const std::string& clientId)
{
  qDebug("[%s] url=%s, id=%s", __func__, sfuUrl.c_str(), clientId.c_str());

  try {
    std::string ws = sfuUrl + "/?clientId=" + clientId;
    websocketpp::lib::error_code ec;

    // Get new connection
    connection = client.get_connection(ws, ec);
    // Check error
    if (ec) {
      Log("get_connection failed. " + std::to_string(ec.value()) + ec.message());
      return false;
    }

    // Register our message handler
    connection->set_message_handler([&](websocketpp::connection_hdl con, websocketpp::config::asio_client::message_type::ptr frame) {
      // Pass to the sfu client
      this->callback_(frame->get_payload());
    });

    // Set close hanlder
    connection->set_close_handler([&](...) {
      // Call listener
      Log("Connection close handler");
      // Don't wait for connection close
      thread.detach();
      // Remove connection
      connection = nullptr;
    });

    // Set failure handler
    connection->set_fail_handler([&](...) {
      // Call listener
      Log("Connection fail handler");
      // Don't wait for connection close
      thread.detach();
      // Remove connection
      connection = nullptr;
    });


    connection->set_open_handler([=](websocketpp::connection_hdl con) {
      // Launch event
      Log("CONNECTED");
    });

    // Note that connect here only requests a connection. No network messages
    // are exchanged until the event loop starts running in the next line.
    client.connect(connection);

    // Async
    thread = std::thread([&]() {
      //Run client async
      client.run();
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
    client.close(connection, websocketpp::close::status::normal, std::string("disconnect"));
    connection = nullptr;
    Log("DISCONNECTED");
  } catch (websocketpp::exception const &e) {
    Log("[EXCEPTION] close:");
    Log(e.what());
    return false;
  }

  return true;
}

void Controller::onConnectedSfu()
{
  Log("Successfully connect to SFU");
  state = State::Connected;
}

void Controller::onDisconnectedSfu()
{
  qDebug("[%s]", __func__);
  state = State::Disconnected;
}

void Controller::send(const std::string &message)
{
  qDebug("[%s] msg=\"%s\"", __func__, message.c_str());
  try {
    // Send it
    connection->send(message);
  } catch (...) {
    Log("[ERROR]Controller::send() exception");
  }
}

void Controller::onStreamPublished()
{
  qDebug("[%s]", __func__);
}
void Controller::onStreamUnpublished(const std::string &streamId)
{
  qDebug("[%s]", __func__);
}
void Controller::onParticipantJoined(const std::string &roomId, const std::string &clientId, const std::string &reason)
{
  qDebug("[%s]", __func__);
}
void Controller::onParticipantLeft(const std::string &roomId, const std::string &clientId, const std::string &reason)
{
  qDebug("[%s]", __func__);
}
void Controller::onParticipantKicked(const std::string &roomId, const std::string &reason)
{
  qDebug("[%s]", __func__);
}
void Controller::onActiveSpeakerChanged(const std::string &roomId, const std::string &clientId)
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
}

void Controller::publishCamera()
{
  qDebug("[%s]", __func__);
}

void Controller::publishDesktop()
{
  state = State::Desktop;
}

void Controller::Log(const std::string &log)
{
  if (logger) logger(log);
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
}
void Controller::onSetRemoteDescriptionSuccess()
{
  qDebug("[%s]", __func__);
}
void Controller::onCreatedAnswerSuccess(const QJsonObject &sdp)
{
  qDebug("[%s]", __func__);
}
