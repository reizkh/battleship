#ifndef BATTLESHIP_USER_IO_H
#define BATTLESHIP_USER_IO_H

#include "common.h"
#include "client.h"

class UserIO {
 public:
  UserIO();
  ~UserIO();
  static void Run();
  static void Start(std::shared_ptr<Client>&);
  static void Draw(int, int, const Grid&);
};
#endif  // BATTLESHIP_USER_IO_H
