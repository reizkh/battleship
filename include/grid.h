#ifndef BATTLESHIP_INCLUDE_BOARD_H_
#define BATTLESHIP_INCLUDE_BOARD_H_

#include "common.h"

enum class CellInfo : char {
  Ship = '%',
  Empty = '.',
  Unknown = '*',
  Hit = 'x',
  Miss = 'o'
};

class Grid {
 public:
  Grid(CellInfo, size_t);
  bool ChangeState(int, int, CellInfo);
  void GroupChangeState(int, int, int, int, CellInfo);
  bool TryPlace(int, int, int, int);
  bool IsDefeated() const;
  CellInfo GetState(int, int) const;
  size_t Size() const;
  std::vector<int> GetShip(int, int) const;
  bool IsDead(int, int) const;

 private:
  size_t size;
  std::vector<std::vector<CellInfo>> grid_data_;
  bool IsShip(CellInfo) const;
  bool IsSafe(int) const;
  bool IsSafe(int, int) const;
};

#endif  // BATTLESHIP_INCLUDE_BOARD_H_
