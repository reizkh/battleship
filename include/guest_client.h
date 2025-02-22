#ifndef BATTLESHIP_INCLUDE_GUEST_CLIENT_H_
#define BATTLESHIP_INCLUDE_GUEST_CLIENT_H_

#include "client.h"

class GuestClient : public virtual Client {
 public:
  GuestClient(const std::string&, uint16_t);
  ~GuestClient() override;
  void Connect() override;

 private:
  tcp::endpoint endpoint_;
};

#endif  // BATTLESHIP_INCLUDE_GUEST_CLIENT_H_
