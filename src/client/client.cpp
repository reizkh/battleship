#include "client.h"

#include "common.h"
#include "connection.h"
#include "message.h"

void Client::ReceiveMessage(Message& msg) {
  Message response{MessageType::BadRequest};
  switch (msg.type_) {
    case MessageType::PlayerTurn:
      int x;
      int y;
      msg >> &x >> &y;
      response << y << x;
       if (local_grid_.GetState(y, x) == CellInfo::Ship) {
        local_grid_.ChangeState(y, x, CellInfo::Hit);
        response.type_ = MessageType::ResultHit;
      } else {
        response.type_ = MessageType::ResultMiss;
      }
      connection_->SendMessage(response);
      break;
    case MessageType::SetupReady:
      remote_ready_ = true;
      if (local_ready_) {
        state_ = GameState::GameOurTurn;
      }
      break;
    case MessageType::ResultHit:
      msg >> &x >> &y;
      remote_grid_.ChangeState(y, x, CellInfo::Hit);
      state_ = GameState::GameTheirTurn;
      break;
    case MessageType::ResultMiss:
      msg >> &x >> &y;
      remote_grid_.ChangeState(y, x, CellInfo::Empty);
      state_ = GameState::GameTheirTurn;
      break;
    case MessageType::BadRequest:
      break;
  }
}

void Client::OnConnection(const asio::error_code& ec) {
  if (!ec) {
    connection_->WaitForIncomingMessage();
    state_ = GameState::Setup;
  } else {
  }
}

Client::~Client() {
  if (thread_context_.joinable()) {
    thread_context_.join();
  }
}

void Client::Hit(int y, int x) {
  Message msg(MessageType::PlayerTurn);
  msg << y << x;
  connection_->SendMessage(msg);
  state_ = GameState::Waiting;
}

void Client::CompleteSetup() {
  local_ready_ = true;
  connection_->SendMessage({MessageType::SetupReady});
  if (remote_ready_) {
    state_ = GameState::GameTheirTurn;
  }
}
