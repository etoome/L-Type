#include "client/cli/assets/Henchman_3_right.hpp"

void Henchman_3_right::draw(WINDOW* win, int y, int x) const {
  mvwprintw(win, y, x, "👾");
}
