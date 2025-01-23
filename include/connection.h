#ifndef BATTLESHIP_LIBS_CONNECTION_H_
#define BATTLESHIP_LIBS_CONNECTION_H_

#include "common.h"
#include "message.h"

class IMessageReceiver {
 public:
  virtual void ReceiveMessage(Message&) = 0;
};

class Connection {
 public:
  Connection(tcp::socket&&, asio::io_context&, IMessageReceiver* owner);
  void SendMessage(const Message&);
  void WaitForIncomingMessage();
  void Close();
  std::string GetPeerAddress();
  uint16_t GetPeerPort();

  tcp::socket socket_;
 private:
  IMessageReceiver* owner_;
  asio::io_context& context_;
  std::vector<char> buffer_;
};

#endif  // BATTLESHIP_LIBS_CONNECTION_H_
