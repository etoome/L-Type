#include "client/cli/assets/Bullet.hpp"

void Bullet::draw(WINDOW* win, int y, int x) const {
  mvwprintw(win, y, x, "•");
}
