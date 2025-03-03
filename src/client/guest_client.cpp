#include "guest_client.h"

using namespace std::placeholders;

GuestClient::GuestClient(const std::string& address, uint16_t port)
    : endpoint_(asio::ip::make_address(address), port) {}

void GuestClient::Connect() {
  auto self = shared_from_this();
  connection_ = std::make_shared<Connection>(tcp::socket(context_), context_, self);
  connection_->socket_.open(endpoint_.protocol());
  connection_->socket_.async_connect(endpoint_, std::bind(&GuestClient::OnConnection, this, _1));
  thread_context_ = std::thread{[this]() { context_.run(); }};
}

GuestClient::~GuestClient() = default;
