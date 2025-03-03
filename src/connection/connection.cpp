#include "connection.h"
#include "message.h"

#include <asio/write.hpp>
#include <asio/read.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/portable_binary.hpp>

Connection::Connection(
  tcp::socket&& socket,
  asio::io_context& context,
  std::shared_ptr<MessageReceiver> owner
) : socket_(std::move(socket))
  , context_(context)
  , owner_(owner)
{}

void Connection::SendMessage(std::unique_ptr<Message> msg) {
  {
    std::ostream stream(&output_buffer_);
    cereal::PortableBinaryOutputArchive ar(stream);
    ar(msg);
  }
  output_header_ = output_buffer_.size();
  SendHeader();
}

void Connection::SendHeader() {
  auto self = shared_from_this();
  output_header_ = htonl(output_header_);
  asio::async_write(
    socket_,
    asio::buffer(&output_header_, sizeof(output_header_)),
    asio::transfer_all(),
    [this, self](const asio::error_code& ec, std::size_t bytes) {
      if (!ec) {
        SendBody();
      }
    }
  );
}

void Connection::SendBody() {
  auto self = shared_from_this();
  asio::async_write(
    socket_,
    output_buffer_,
    asio::transfer_exactly(output_header_),
    [this, self](const asio::error_code& ec, std::size_t bytes) {
      if (!ec) {
        output_buffer_.consume(bytes);
      }
    }
  );
}

void Connection::HandleIncomingMessages() {
  ReadHeader();
}

void Connection::ReadHeader() {
  auto self = shared_from_this();
  asio::async_read(
    socket_,
    asio::buffer(&input_header_, sizeof(input_header_)),
    asio::transfer_all(),
    [this, self](const asio::error_code& ec, std::size_t bytes) {
      if (!ec) {
        input_header_ = ntohl(input_header_);
        ReadBody();
      }
    }
  );
}


void Connection::ReadBody() {
  auto self = shared_from_this();
  asio::async_read(
    socket_,
    input_buffer_,
    asio::transfer_exactly(input_header_),
    [this, self](const asio::error_code& ec, std::size_t bytes) {
      std::unique_ptr<Message> msg;
      {
        std::istream stream(&input_buffer_);
        cereal::PortableBinaryInputArchive ar(stream);
        ar(msg);
        input_buffer_.consume(bytes);
      }
      msg->Accept(owner_);
      ReadHeader();
    }
  );
}
