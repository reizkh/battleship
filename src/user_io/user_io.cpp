#include "user_io.h"

#include <guest_client.h>
#include <host_client.h>
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

void Status(const char* message) {
  int row = getmaxy(stdscr);
  mvprintw(row - 1, 0, "%s", message);
  refresh();
}

void UserIO::Start(std::shared_ptr<Client>& c) {
  int row;
  int col;
  int menu_w = 30;
  int menu_h = 7;
  getmaxyx(stdscr, row, col);
  WINDOW* menu_box = newwin(menu_h, menu_w, 0, 0);
  int choice;
  do {
    wclear(menu_box);
    wrefresh(menu_box);
    getmaxyx(stdscr, row, col);
    box(menu_box, ACS_VLINE, ACS_HLINE);
    mvwin(menu_box, (row - menu_h) / 2, (col - menu_w) / 2);
    mvwaddstr(menu_box, 1, 1, "(H)ost a new game");
    mvwaddstr(menu_box, 3, 1, "(C)onnect to a game");

    choice = wgetch(menu_box);
  } while (choice != 'H' && choice != 'C');
  c = std::make_shared<HostClient>(60000);
}

void UserIO::Run() {
  std::shared_ptr<Client> client;
  Start(client);

  int grid_size = static_cast<int>(client->local_grid_.Size());
  int row;
  int col;

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
        Status("Other player ready");
        notified_ready = true;
      }
      getmaxyx(stdscr, row, col);
      int ch = getch();

      if (ch != ERR) {
        wclear(stdscr);
        highlight = true;
        if (ch == KEY_UP) --pos_y;
        if (ch == KEY_DOWN) ++pos_y;
        if (ch == KEY_RIGHT) ++pos_x;
        if (ch == KEY_LEFT) --pos_x;
        if (ch == ' ') std::swap(width, height);
        if (ch == '\n') {
          if (client->local_grid_.TryPlace(pos_y, pos_x, height, width)) {
            placed = true;
            highlight = false;
          } else {
            Status("Ships cannot intersect");
          }
        }
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

  while (client->state_ == GameState::Setup) {
    Wait("Waiting for them to finish setup");
  }

  int width = 1;
  int height = 1;
  while (true) {
    while (client->state_ == GameState::Waiting) {
      Wait("Waiting the response");
    }
    // Game over after our turn means we win
    if (client->state_ == GameState::GameOver) {
      Status("Game over. You win! Press any key to exit.");
      break;
    }

    wclear(stdscr);
    Draw(0, 0, client->local_grid_);
    Draw(row - grid_size, col - grid_size, client->remote_grid_);

    while (client->state_ == GameState::GameTheirTurn) {
      Wait("Waiting for their turn");
    }
    // Game over after their turn means they win
    if (client->state_ == GameState::GameOver) {
      Status("Game over. You lose! Press any key to exit.");
      break;
    }

    bool placed = false;
    while (!placed) {
      getmaxyx(stdscr, row, col);
      int ch = getch();
      wclear(stdscr);
      if (ch != ERR) {
        highlight = true;
        if (ch == KEY_UP) --pos_y;
        if (ch == KEY_DOWN) ++pos_y;
        if (ch == KEY_RIGHT) ++pos_x;
        if (ch == KEY_LEFT) --pos_x;
        if (ch == '\n') {
          if (client->remote_grid_.GetState(pos_y, pos_x) ==
              CellInfo::Unknown) {
            placed = true;
            highlight = false;
            client->Hit(pos_y, pos_x);
          } else {
            Status("You cannot hit the same cell twice");
          }
        }
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
  while (getch() == ERR) {
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
