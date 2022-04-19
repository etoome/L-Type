#include "client/cli/assets/Henchman_2_right.hpp"

void Henchman_2_right::draw(WINDOW* win, int y, int x) const {
  mvwprintw(win, y, x, "👾");
}
