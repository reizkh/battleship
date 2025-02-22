#include "client.h"
#include "message.h"

void Client::HandleMessage(TurnPosition& msg) {
  if (local_grid_.GetState(msg.y, msg.x) != CellInfo::Ship) {   // miss case
    local_grid_.ChangeState(msg.y, msg.x, CellInfo::Miss);
    state_ = GameState::GameOurTurn;
    connection_->SendMessage(std::make_unique<Miss>(msg.x, msg.y));
    return;
  }

  local_grid_.ChangeState(msg.y, msg.x, CellInfo::Dead);

  if (local_grid_.IsDefeated()) {                               // loss case
    state_ = GameState::GameOver;
    connection_->SendMessage(std::make_unique<LastShipDestroyed>());
    return;
  }
  if (local_grid_.IsDead(msg.y, msg.x)) {                       // dead ship case
    auto ship = local_grid_.GetShip(msg.y, msg.x);
    state_ = GameState::GameTheirTurn;
    connection_->SendMessage(std::make_unique<Kill>(ship[1], ship[2], ship[3], ship[0]));
  } else {                                                      // ship survived case
    state_ = GameState::GameTheirTurn;
    connection_->SendMessage(std::make_unique<Hit>(msg.x, msg.y));
  }
}

void Client::HandleMessage(SetupReady&) {
  remote_ready_ = true;
  if (local_ready_) state_ = GameState::GameOurTurn;
}

void Client::HandleMessage(Hit& msg) {
  remote_grid_.ChangeState(msg.y, msg.x, CellInfo::Dead);
  state_ = GameState::GameOurTurn;
}

void Client::HandleMessage(Miss& msg) {
  remote_grid_.ChangeState(msg.y, msg.x, CellInfo::Miss);
  state_ = GameState::GameTheirTurn;
}

void Client::HandleMessage(Kill& msg) {
  remote_grid_.GroupChangeState(msg.y1 + 1, msg.x1 - 1, msg.y2 - 1, msg.x2 + 1, CellInfo::Empty);
  remote_grid_.GroupChangeState(msg.y1, msg.x1, msg.y2, msg.x2, CellInfo::Dead);
  state_ = GameState::GameOurTurn;
}

void Client::HandleMessage(LastShipDestroyed&) {
  state_ = GameState::GameOver;
}

void Client::SendTurn(std::int32_t y, std::int32_t x) {
  connection_->SendMessage(std::make_unique<TurnPosition>(x, y));
  state_ = GameState::Waiting;
}

void Client::OnConnection(const asio::error_code& ec) {
  if (!ec) {
    connection_->HandleIncomingMessages();
    state_ = GameState::Setup;
  }
}

Client::~Client() {
  context_.stop();
  if (thread_context_.joinable()) {
    thread_context_.join();
  }
}

void Client::CompleteSetup() {
  local_ready_ = true;
  connection_->SendMessage(std::make_unique<SetupReady>());
  if (remote_ready_) {
    state_ = GameState::GameTheirTurn;
  }
}
