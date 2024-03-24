#include "connection.h"

#include "common.h"
#include "message.h"

Connection::Connection(tcp::socket&& sock, asio::io_context& context,
                       IMessageReceiver* owner)
    : socket_(std::move(sock)), context_(context), owner_(owner), buffer_(1024) {}

void Connection::SendMessage(const Message& msg) {
  char* ptr = buffer_.data();

  memcpy(ptr, &msg.type_, sizeof(MessageType));
  ptr += sizeof(MessageType);
  memcpy(ptr, &msg.body_size_, sizeof(msg.body_size_));
  ptr += sizeof(msg.body_size_);
  memcpy(ptr, msg.body_.data(), msg.body_size_);

  socket_.async_write_some(asio::buffer(buffer_.data(), buffer_.size()), [&](const asio::error_code&, size_t){});
}

void Connection::WaitForIncomingMessage() {
  socket_.async_read_some(
      asio::buffer(buffer_.data(), buffer_.size()),
      [this](const asio::error_code& ec, size_t len) {
        if (!ec) {
          MessageType msg_type =
              *reinterpret_cast<MessageType*>(buffer_.data());
          Message msg(msg_type);
          owner_->ReceiveMessage(msg);
        } else {
          Close();
        }
        WaitForIncomingMessage();
      });
}

void Connection::Close() {
  if (socket_.is_open()) {
    socket_.close();
  }
  if (!context_.stopped()) {
    context_.stop();
  }
}

std::string Connection::GetPeerAddress() {
  return socket_.remote_endpoint().address().to_string();
}

uint16_t Connection::GetPeerPort() { return socket_.remote_endpoint().port(); }