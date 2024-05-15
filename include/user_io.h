#ifndef BATTLESHIP_USER_IO_H
#define BATTLESHIP_USER_IO_H

#include "common.h"
#include "client.h"

class UserIO {
 public:
  UserIO();
  ~UserIO();
  void Wait(const char*, size_t) const;
  void Run() const;
  void Start(std::shared_ptr<Client>&) const;
  static void Draw(int, int, const Grid&);
  void Status(const char*) const;
  void* status_bar;
};
#endif  // BATTLESHIP_USER_IO_H
