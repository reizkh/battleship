#include "grid.h"

Grid::Grid(CellInfo def, size_t size) : grid_data_(size, {size, def}) {}

bool Grid::ChangeState(int x, int y, CellInfo new_state) {
  if (x > grid_data_.size() || y > grid_data_.size()) {
    return false;
  }
  grid_data_[x][y] = new_state;
  return true;
}

CellInfo Grid::GetState(int y, int x) const { return grid_data_[y][x]; }

bool Grid::TryPlace(int y, int x, int h, int w) {
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      if (grid_data_[y + i][x + j] == CellInfo::Ship) {
        return false;
      }
    }
  }
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      grid_data_[y + i][x + j] = CellInfo::Ship;
    }
  }
  return true;
}

size_t Grid::Size() const { return grid_data_.size(); }
