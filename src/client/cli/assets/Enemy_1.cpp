#include "client/cli/assets/Enemy_1.hpp"

#include "client/cli/assets/Enemy.hpp"

void Enemy_1::draw(WINDOW* win, int y, int x) const {
  wattron(win, COLOR_PAIR(3));
  mvwprintw(win, y, x, "▌");
  mvwprintw(win, y, x + 1, "+");
  mvwprintw(win, y, x + 2, "▐");
  wattroff(win, COLOR_PAIR(3));
}
