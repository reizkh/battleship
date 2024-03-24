#ifndef BATTLESHIP_INCLUDE_BOARD_H_
#define BATTLESHIP_INCLUDE_BOARD_H_

#include "common.h"

enum class CellInfo : char {
  Ship = '%',
  Empty = '.',
  Unknown = '*',
  Hit = 'x'
};

class Grid {
 public:
  Grid(CellInfo, size_t);
  bool ChangeState(int, int, CellInfo);
  bool TryPlace(int, int, int, int);
  bool IsDefeated() const;
  CellInfo GetState(int, int) const;
  size_t Size() const;

 private:
  std::vector<std::vector<CellInfo>> grid_data_;
};

#endif  // BATTLESHIP_INCLUDE_BOARD_H_
