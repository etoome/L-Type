#include "client/cli/assets/Obstacle_2.hpp"

#include "client/cli/assets/Obstacle.hpp"

void Obstacle_2::draw(WINDOW* win, int y, int x) const {
  for (int i = 0; unsigned(i) != _height; ++i) {
    mvwprintw(win, y + i, x, "|");
    mvwprintw(win, y + i, x + int(_width) - 1, "|");
  }
  for (int i = 0; unsigned(i) != _width; ++i) {
    mvwprintw(win, y, x + i, "-");
    mvwprintw(win, y - 1 + int(_height), x + i, "-");
  }
}
