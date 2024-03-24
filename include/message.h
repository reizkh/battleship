#ifndef BATTLESHIP_LIBS_MESSAGE_H_
#define BATTLESHIP_LIBS_MESSAGE_H_

#include "common.h"

enum class MessageType : uint32_t {
  SetupReady,
  PlayerTurn,
  ResultHit,
  ResultMiss,
  BadRequest
};

class Message {
 public:
  Message(const MessageType&);

  template <class T>
  friend Message& operator<<(Message& msg, const T& arg) {
    static_assert(std::is_standard_layout_v<T>);

    msg.body_.resize(msg.body_.size() + sizeof(T));

    memcpy(msg.body_.data() + msg.body_.size(), &arg, sizeof(T));
    msg.body_size_ += msg.body_.size();
    return msg;
  }

  template <class T>
  friend Message& operator>>(Message& msg, T* arg) {
    static_assert(std::is_standard_layout_v<T>);

    msg.body_.resize(msg.body_.size() - sizeof(T));

    memcpy(arg, msg.body_.data() + msg.body_.size(), sizeof(T));
    return msg;
  }

 public:
  MessageType type_;
  uint32_t body_size_{0};
  std::vector<char> body_{0};

};

#endif  // BATTLESHIP_LIBS_MESSAGE_H_
