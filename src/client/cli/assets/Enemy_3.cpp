#include "client/cli/assets/Enemy_3.hpp"

#include "client/cli/assets/Enemy.hpp"

void Enemy_3::draw(WINDOW* win, int y, int x) const {
  wattron(win, COLOR_PAIR(3));
  mvwprintw(win, y, x, "|");
  mvwprintw(win, y + 1, x, "|");
  mvwprintw(win, y, x + 1, "_");
  mvwprintw(win, y + 2, x, "|");
  mvwprintw(win, y + 2, x + 1, "_");
  mvwprintw(win, y + 1, x + 1, "⋄");
  mvwprintw(win, y, x + 2, "|");
  mvwprintw(win, y + 1, x + 2, "|");
  mvwprintw(win, y + 2, x + 2, "|");
  wattroff(win, COLOR_PAIR(3));
}
