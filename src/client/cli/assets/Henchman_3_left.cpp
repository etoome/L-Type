#include "client/cli/assets/Henchman_3_left.hpp"

void Henchman_3_left::draw(WINDOW* win, int y, int x) const {
  mvwprintw(win, y, x, "👾");
}
