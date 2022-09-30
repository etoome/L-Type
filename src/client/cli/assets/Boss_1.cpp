#include "client/cli/assets/Boss_1.hpp"

void Boss_1::draw(WINDOW* win, int y, int x) const {
  wattron(win, COLOR_PAIR(4));
  for (int i = 0; i != int(_height); ++i) {
    if (i % 2 == 0) {
      mvwprintw(win, y + i, x, "/");
      mvwprintw(win, y, x + int(_width), "/");
    } else {
      mvwprintw(win, y + i, x, "\\");
      mvwprintw(win, y + i, x + int(_width), "\\");
    }
    wattron(win, COLOR_PAIR(0));
    for (int v = 1; v != int(_width) - 1; ++v) {
      mvwprintw(win, y + i, x + v, "◆");
    }
    wattroff(win, COLOR_PAIR(0));
  }
  wattroff(win, COLOR_PAIR(4));
  wattron(win, COLOR_PAIR(3));
  mvwprintw(win, y + 2, x + 1, "👁");
  mvwprintw(win, y + 2, x + 4, "👁");
  wattroff(win, COLOR_PAIR(3));
}
