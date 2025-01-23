#include "client.h"

#include "common.h"
#include "message.h"

void Client::ReceiveMessage(Message& msg) {
  Message response{MessageType::BadRequest};
  uint16_t y1;
  uint16_t x1;
  uint16_t y2;
  uint16_t x2;
  switch (msg.type_) {
    case MessageType::MoveDone:
      msg >> &x1 >> &y1;
      if (local_grid_.GetState(y1, x1) != CellInfo::Ship) {             // miss case
        local_grid_.ChangeState(y1, x1, CellInfo::Miss);
        response << y1 << x1;
        state_ = GameState::GameOurTurn;
        response.type_ = MessageType::ResultMiss;
        connection_->SendMessage(response);
        break;
      }
      local_grid_.ChangeState(y1, x1, CellInfo::Hit);
      if (local_grid_.IsDefeated()) {                                   // loss case
        state_ = GameState::GameOver;
        response.type_ = MessageType::ResultWin;
        connection_->SendMessage(response);
        break;
      }
      if (local_grid_.IsDead(y1, x1)) {                                 // dead ship case
        for (auto coord : local_grid_.GetShip(y1, x1)) { response << static_cast<uint16_t>(coord); }
        state_ = GameState::GameTheirTurn;
        response.type_ = MessageType::ResultKill;
        connection_->SendMessage(response);
        break;
      }
      {
        response << y1 << x1;                                           // ship survived case
        state_ = GameState::GameTheirTurn;
        response.type_ = MessageType::ResultHit;
        connection_->SendMessage(response);
        break;
      }
    case MessageType::SetupReady:
      remote_ready_ = true;
      if (local_ready_) {
        state_ = GameState::GameOurTurn;
      }
      break;
    case MessageType::ResultKill:
      msg >> &x1 >> &y1 >> &x2 >> &y2;
      if (x1 > x2) {std::swap(x1, x2);}
      if (y1 > y2) {std::swap(y1, y2);}
      remote_grid_.GroupChangeState(y1 - 1, x1 - 1, y2 + 1, x2 + 1, CellInfo::Empty);
      remote_grid_.GroupChangeState(y1, x1, y2, x2, CellInfo::Hit);
      state_ = GameState::GameOurTurn;
      break;
    case MessageType::ResultHit:
      msg >> &x1 >> &y1;
      remote_grid_.ChangeState(y1, x1, CellInfo::Hit);
      state_ = GameState::GameOurTurn;
      break;
    case MessageType::ResultMiss:
      msg >> &x1 >> &y1;
      remote_grid_.ChangeState(y1, x1, CellInfo::Miss);
      state_ = GameState::GameTheirTurn;
      break;
    case MessageType::ResultWin:
      state_ = GameState::GameOver;
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
  context_.stop();
  if (thread_context_.joinable()) {
    thread_context_.join();
  }
  connection_.release();
}

void Client::Hit(uint16_t y, uint16_t x) {
  Message msg(MessageType::MoveDone);
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
