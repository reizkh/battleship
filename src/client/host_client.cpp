#include "host_client.h"

#include <memory>

HostClient::HostClient(uint16_t port) : Client(), acceptor_(context_, tcp::endpoint(tcp::v4(), port)) {}

void HostClient::Connect() {
  try {
    acceptor_.async_accept([&](const asio::error_code& ec, tcp::socket&& socket) {
          if (!ec) {
            connection_ =
                std::make_unique<Connection>(std::move(socket), context_, this);
            OnConnection(ec);
          } else {
          }
        });

    thread_context_ = std::thread{[this]() { context_.run(); }};
  } catch (std::exception& e) {
    return;
  }
}

std::shared_ptr<HostClient> HostClient::Create(uint16_t port) {
  return std::shared_ptr<HostClient>(new HostClient(port));
}

HostClient::~HostClient() = default;
