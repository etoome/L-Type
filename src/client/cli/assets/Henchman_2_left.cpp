#include "client/cli/assets/Henchman_2_left.hpp"

void Henchman_2_left::draw(WINDOW* win, int y, int x) const {
  mvwprintw(win, y, x, "👾");
}
