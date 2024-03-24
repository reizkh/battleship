#include "guest_client.h"

GuestClient::GuestClient(const std::string& address, uint16_t port)
    : endpoint_(asio::ip::make_address(address), port) {}

void GuestClient::Connect() {
  connection_ = std::make_shared<Connection>(tcp::socket(context_), context_, this);
  connection_->socket_.open(endpoint_.protocol());
  connection_->socket_.async_connect(endpoint_,
                      [&](const asio::error_code& ec) { OnConnection(ec); });
  thread_context_ = std::thread{[&]() { context_.run(); }};
}

std::shared_ptr<GuestClient> GuestClient::Create(const std::string& address,
                                                 uint16_t port) {
  return std::shared_ptr<GuestClient>(new GuestClient(address, port));
}
GuestClient::~GuestClient() = default;
