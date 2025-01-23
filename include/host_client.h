#ifndef BATTLESHIP_INCLUDE_HOST_CLIENT_H_
#define BATTLESHIP_INCLUDE_HOST_CLIENT_H_

#include "client.h"

class HostClient : public virtual Client {
 protected:
  HostClient(uint16_t);

 public:
  ~HostClient() override;
  static std::shared_ptr<HostClient> Create(uint16_t);
  void Connect() override;

 private:
  tcp::acceptor acceptor_;
};

#endif  // BATTLESHIP_INCLUDE_HOST_CLIENT_H_
