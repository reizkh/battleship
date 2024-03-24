#ifndef BATTLESHIP_INCLUDE_CLIENT_H_
#define BATTLESHIP_INCLUDE_CLIENT_H_

#include "common.h"
#include "connection.h"
#include "grid.h"

enum class GameState {
  MakingConnection,
  Setup,
  GameOurTurn,
  GameTheirTurn,
  Waiting,
  GameOver,
};

class Client : public IMessageReceiver {
 public:
  virtual ~Client();
  virtual void Connect() = 0;
  void OnConnection(const asio::error_code&);
  void CompleteSetup();
  void ReceiveMessage(Message& msg) override;
  void Hit(uint16_t, uint16_t);

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
