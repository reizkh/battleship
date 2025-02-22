#ifndef BATTLESHIP_INCLUDE_CLIENT_H_
#define BATTLESHIP_INCLUDE_CLIENT_H_

#include "message.h"
#include "connection.h"
#include "grid.h"

#include <thread>

enum class GameState {
  MakingConnection,
  Setup,
  GameOurTurn,
  GameTheirTurn,
  Waiting,
  GameOver,
};

class Client : public MessageReceiver {
 public:
  virtual ~Client();
  virtual void Connect() = 0;
  void OnConnection(const asio::error_code&);
  void CompleteSetup();
  void SendTurn(std::int32_t, std::int32_t);

  void HandleMessage(TurnPosition&) override;
  void HandleMessage(LastShipDestroyed&) override;
  void HandleMessage(Hit&) override;
  void HandleMessage(Miss&) override;
  void HandleMessage(Kill&) override;
  void HandleMessage(SetupReady&) override;

 public:
  GameState state_ = GameState::MakingConnection;
  Grid local_grid_{CellInfo::Empty, 10};
  Grid remote_grid_{CellInfo::Unknown, 10};
  bool remote_ready_ = false;
  bool local_ready_ = false;

 protected:
  asio::io_context context_;
  std::shared_ptr<Connection> connection_;
  std::thread thread_context_;
};

#endif  // BATTLESHIP_INCLUDE_CLIENT_H_
