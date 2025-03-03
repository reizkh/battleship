#ifndef BATTLESHIP_INCLUDE_HOST_CLIENT_H_
#define BATTLESHIP_INCLUDE_HOST_CLIENT_H_

#include "client.h"

class HostClient : public virtual Client {
 public:
  HostClient(uint16_t);
  ~HostClient() override;
  void Connect() override;

 private:
  tcp::acceptor acceptor_;
};

#endif  // BATTLESHIP_INCLUDE_HOST_CLIENT_H_
