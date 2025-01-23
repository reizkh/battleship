#include "grid.h"

Grid::Grid(CellInfo def, size_t init_size) : size(init_size), grid_data_(size, {size, def}) {}

bool Grid::IsShip(CellInfo value) const { return value == CellInfo::Hit || value == CellInfo::Ship; }

bool Grid::IsSafe(int coord) const { return (coord >= 0 && coord < size); }

bool Grid::IsSafe(int y, int x) const { return IsSafe(y) && IsSafe(x); }

bool Grid::ChangeState(int y, int x, CellInfo new_state) {
  if (IsSafe(y, x)) {
    grid_data_[y][x] = new_state;
    return true;
  }
  return false;
}

void Grid::GroupChangeState(int y1, int x1, int y2, int x2, CellInfo new_state) {
  if (y1 > y2) {std::swap(y1, y2);}
  if (x1 > x2) {std::swap(x1, x2);}
  for (int y = y1; y <= y2; ++y) {
    for (int x = x1; x <= x2; ++x) {
      ChangeState(y, x, new_state);
    }
  }
}

bool Grid::TryPlace(int py, int px, int h, int w) {
  for (int y = py - 1; y <= py + h; ++y) {
    for (int x = px - 1; x <= px + w; ++x) {
      if (!IsSafe(y, x)) { continue; }
      if (IsShip(grid_data_[y][x])) {
        return false;
      }
    }
  }
  GroupChangeState(py, px, py + h - 1, px + w - 1, CellInfo::Ship);
  return true;
}

bool Grid::IsDefeated() const {
  for (int y = 0; y < size; ++y) {
    for (int x = 0; x < size; ++x) {
      if (grid_data_[y][x] == CellInfo::Ship) {
        return false;
      }
    }
  }
  return true;
}

CellInfo Grid::GetState(int y, int x) const { return grid_data_[y][x]; }

size_t Grid::Size() const { return size; }

std::vector<int> Grid::GetShip(int y, int x) const {
  bool is_horisontal = false;
  for (const auto& sft : std::vector<std::pair<int, int>>({{0, 1}, {0, -1}, {1, 0}, {-1, 0}})) {
    if (IsSafe(y + sft.first, x + sft.second)) {
      if (IsShip(grid_data_[y + sft.first][x + sft.second])) {
        if (sft.first == 0) {
          is_horisontal = true;
        }
        break;
      }
    }
  }
  if (!is_horisontal) {
    int bot = y;
    int top = y;
    while (IsSafe(bot - 1) && IsShip(grid_data_[bot - 1][x])) {
      --bot;
    }
    while (IsSafe(top + 1) && IsShip(grid_data_[top + 1][x])) {
      ++top;
    }
    return {bot, x, top, x};
  } else {
    int lft = x;
    int rgt = x;
    while (IsSafe(lft - 1) && IsShip(grid_data_[y][lft - 1])) {
      --lft;
    }
    while (IsSafe(rgt + 1) && IsShip(grid_data_[y][rgt + 1])) {
      ++rgt;
    }
    return {y, lft, y, rgt};
  }
}

bool Grid::IsDead(int cy, int cx) const {
  if (!IsShip(grid_data_[cy][cx])) {
    return false;
  }
  std::vector<int> coords = GetShip(cy, cx);
  for (int y = coords[0]; y <= coords[2]; ++y) {
    for (int x = coords[1]; x <= coords[3]; ++x) {
      if (grid_data_[y][x] != CellInfo::Hit) {
        return false;
      }
    }
  }
  return true;
}




