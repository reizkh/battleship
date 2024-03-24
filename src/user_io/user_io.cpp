#include "user_io.h"

#include <ncurses.h>

#include <iostream>
#include <vector>

#include "grid.h"

void Wait(const char* prompt, int max_dots = 3) {
  static int dots = 0;

  int row = getmaxy(stdscr);

  std::this_thread::sleep_for(300ms);

  mvprintw(row - 1, 0, "%s", prompt);
  if (dots == 0) {
    mvprintw(row - 1, (int)strlen(prompt), "   ");
  } else {
    mvaddch(row - 1, (int)strlen(prompt) + dots - 1, '.');
  }
  dots = ++dots % (max_dots + 1);
  refresh();
}

void UserIO::Run(const std::shared_ptr<Client>& client) {
  int grid_size = static_cast<int>(client->local_grid_.Size());
  int row;
  int col;
  getmaxyx(stdscr, row, col);

  while (client->state_ == GameState::MakingConnection) {
    Wait("Connecting");
  }
  clear();
  refresh();

  std::vector<int> ship_lengths{5, 4, 3, 3, 2};
  bool highlight = false;

  int pos_x = 0;
  int pos_y = 0;
  bool notified_ready = false;

  for (const auto& len : ship_lengths) {
    bool placed = false;
    int width = 1;
    int height = len;

    while (!placed) {
      if (client->remote_ready_ && !notified_ready) {
        mvprintw(row-1, 0, "Other player ready");
        notified_ready = true;
      }
      getmaxyx(stdscr, row, col);
      int ch = getch();

      if (ch != ERR) {
        wclear(stdscr);
        switch (ch) {
          case KEY_UP:
            --pos_y;
            break;
          case KEY_DOWN:
            ++pos_y;
            break;
          case KEY_RIGHT:
            ++pos_x;
            break;
          case KEY_LEFT:
            --pos_x;
            break;
          case ' ':
            std::swap(width, height);
            break;
          case '\n':
            if (client->local_grid_.TryPlace(pos_y, pos_x, height, width)) {
              placed = true;
            } else {
              mvprintw(row - 1, 0, "Ships cannot intersect");
            }
            break;
          default:
            break;
        }
        highlight = true;
      }

      pos_x = std::max(std::min(pos_x, 10 - width), 0);
      pos_y = std::max(std::min(pos_y, 10 - height), 0);

      Draw(0, 0, client->local_grid_);
      Draw(row - grid_size, col - grid_size, client->remote_grid_);

      if (highlight) {
        for (int i = 0; i < height; ++i) {
          for (int j = 0; j < width; ++j) {
            mvaddch(pos_y + i, pos_x + j, '#');
          }
        }
      }
      refresh();
      highlight = !highlight;
    }
  }
  client->CompleteSetup();

  int width = 1;
  int height = 1;
  while (client->state_ != GameState::GameOver) {
    while (client->state_ == GameState::GameTheirTurn) {
      Wait("Waiting for their turn");
    }

    bool placed = false;
    while (!placed) {
      getmaxyx(stdscr, row, col);
      int ch = getch();

      if (ch != ERR) {
        wclear(stdscr);
        switch (ch) {
          case KEY_UP:
            --pos_y;
            break;
          case KEY_DOWN:
            ++pos_y;
            break;
          case KEY_RIGHT:
            ++pos_x;
            break;
          case KEY_LEFT:
            --pos_x;
            break;
          case '\n':
            if (client->remote_grid_.GetState(pos_y, pos_x) ==
                CellInfo::Unknown) {
              placed = true;
              client->Hit(pos_y, pos_x);
              continue;
            } else {
              mvprintw(row - 1, 0, "You cannot hit the same cell twice");
            }
            break;
          default:
            break;
        }
        highlight = true;
      }

      pos_x = std::max(std::min(pos_x, 10 - width), 0);
      pos_y = std::max(std::min(pos_y, 10 - height), 0);

      Draw(0, 0, client->local_grid_);
      Draw(row - grid_size, col - grid_size, client->remote_grid_);

      if (highlight) {
        for (int i = 0; i < height; ++i) {
          for (int j = 0; j < width; ++j) {
            mvaddch(row - grid_size + pos_y + i, col - grid_size + pos_x + j,
                    '#');
          }
        }
      }
      refresh();
      highlight = !highlight;
    }
  }
}

UserIO::UserIO() {
  initscr();
  noecho();
  raw();
  halfdelay(3);
  keypad(stdscr, true);
  curs_set(0);
}

UserIO::~UserIO() { endwin(); }

void UserIO::Draw(int top, int left, const Grid& grid) {
  for (int i = 0; i < grid.Size(); ++i) {
    for (int j = 0; j < grid.Size(); ++j) {
      mvaddch(top + i, left + j, static_cast<char>(grid.GetState(i, j)));
    }
  }
}
