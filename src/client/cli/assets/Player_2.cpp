#include "client/cli/assets/Player_2.hpp"

void Player_2::draw(WINDOW* win, int y, int x) const {
  mvwprintw(win, y, x, "◢");
  mvwprintw(win, y + 1, x, "◥");
  mvwprintw(win, y, x + 1, "◣");
  mvwprintw(win, y + 1, x + 1, "◤");
}
