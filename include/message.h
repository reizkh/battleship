#ifndef BATTLESHIP_LIBS_MESSAGE_H_
#define BATTLESHIP_LIBS_MESSAGE_H_

#include <cereal/types/base_class.hpp>
#include <cereal/types/polymorphic.hpp>

#define CREATE_MESSAGE_TYPE(Name, Base)\
struct Name: Base<Name> { \
  using Base::Base; \
  template <typename Archive> \
  void serialize(Archive& ar) { ar(cereal::base_class<Base<Name>>(this)); } \
}; \
CEREAL_REGISTER_TYPE(Name)

struct MessageReceiver;

struct Message {
  virtual void Accept(std::shared_ptr<MessageReceiver>) {};

  template <typename Archive>
  void serialize(Archive& ar) {}

  virtual ~Message() {}
};

template <typename T>
struct EmptyMessage: Message {
  template <typename Archive>
  void serialize(Archive& ar) {
    ar(cereal::base_class<Message>(this));
  }

  void Accept(std::shared_ptr<MessageReceiver> r) override;
};

template <typename T>
struct PosMessage: Message {
  PosMessage() = default;

  PosMessage(std::int32_t x, std::int32_t y): x(x), y(y) {}

  template <typename Archive>
  void serialize(Archive& ar) {
    ar(cereal::base_class<Message>(this));
    ar(x, y);
  }

  void Accept(std::shared_ptr<MessageReceiver> r) override;

  std::int32_t x;
  std::int32_t y;
};

template <typename T>
struct RectMessage: Message {
  RectMessage() = default;

  RectMessage(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2): x1(x1), y1(y1), x2(x2), y2(y2) {}

  template <typename Archive>
  void serialize(Archive& ar) {
    ar(cereal::base_class<Message>(this));
    ar(x1, y1, x2, y2);
  }

  void Accept(std::shared_ptr<MessageReceiver> r) override;

  std::int32_t x1;
  std::int32_t y1;
  std::int32_t x2;
  std::int32_t y2;
};

CREATE_MESSAGE_TYPE(TurnPosition,       PosMessage);
CREATE_MESSAGE_TYPE(LastShipDestroyed,  EmptyMessage);
CREATE_MESSAGE_TYPE(Miss,               PosMessage);
CREATE_MESSAGE_TYPE(Hit,                PosMessage);
CREATE_MESSAGE_TYPE(Kill,               RectMessage);
CREATE_MESSAGE_TYPE(SetupReady,         EmptyMessage);

struct MessageReceiver: std::enable_shared_from_this<MessageReceiver> {
  virtual ~MessageReceiver() = default;
  virtual void HandleMessage(TurnPosition&) = 0;
  virtual void HandleMessage(LastShipDestroyed&) = 0;
  virtual void HandleMessage(Hit&) = 0;
  virtual void HandleMessage(Miss&) = 0;
  virtual void HandleMessage(Kill&) = 0;
  virtual void HandleMessage(SetupReady&) = 0;
};

template <typename T>
void EmptyMessage<T>::Accept(std::shared_ptr<MessageReceiver> r) { r->HandleMessage(static_cast<T&>(*this)); }

template <typename T>
void PosMessage<T>::Accept(std::shared_ptr<MessageReceiver> r) { r->HandleMessage(static_cast<T&>(*this)); }

template <typename T>
void RectMessage<T>::Accept(std::shared_ptr<MessageReceiver> r) { r->HandleMessage(static_cast<T&>(*this)); }

#endif  // BATTLESHIP_LIBS_MESSAGE_H_
