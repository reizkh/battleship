#include "host_client.h"

#include <memory>

HostClient::HostClient(uint16_t port) : Client(), acceptor_(context_, tcp::endpoint(tcp::v4(), port)) {}

void HostClient::Connect() {
  acceptor_.async_accept([this](const asio::error_code& ec, tcp::socket&& socket) {
    if (!ec) {
      connection_ =
        std::make_shared<Connection>(std::move(socket), context_, shared_from_this());
      OnConnection(ec);
    }
  });

  thread_context_ = std::thread{[this]() { context_.run(); }};
}

HostClient::~HostClient() = default;
