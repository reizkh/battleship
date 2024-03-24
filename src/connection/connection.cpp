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
          char* p = buffer_.data();

          Message msg(reinterpret_cast<MessageType&>(*p));
          p += sizeof(MessageType);

          msg.body_size_ = reinterpret_cast<typeof(msg.body_size_)&>(*p);
          p += sizeof(msg.body_size_);

          msg.body_.resize(msg.body_size_);
          memcpy(msg.body_.data(), p, msg.body_size_);

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