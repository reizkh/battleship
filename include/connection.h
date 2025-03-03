#ifndef BATTLESHIP_LIBS_CONNECTION_H_
#define BATTLESHIP_LIBS_CONNECTION_H_

#include "message.h"

#include <asio/ip/tcp.hpp>
#include <asio/streambuf.hpp>

#include <memory>

using asio::ip::tcp;

class Connection: public std::enable_shared_from_this<Connection> {
 public:
  Connection(tcp::socket&&, asio::io_context&, std::shared_ptr<MessageReceiver>);
  void SendMessage(std::unique_ptr<Message>);
  void HandleIncomingMessages();
  static Connection MakeHost(uint16_t);
  static Connection MakeGuest(const std::string&, uint16_t);

private:
  void SendHeader();
  void SendBody();
  void ReadHeader();
  void ReadBody();

public:
  tcp::socket socket_;
  std::shared_ptr<MessageReceiver> owner_;
  asio::io_context& context_;

 private:
  asio::streambuf input_buffer_;
  std::uint32_t input_header_;
  asio::streambuf output_buffer_;
  std::uint32_t output_header_;
};

#endif  // BATTLESHIP_LIBS_CONNECTION_H_
