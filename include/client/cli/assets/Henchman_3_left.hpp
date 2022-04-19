#pragma once

#include <ncurses.h>

#include "Sprite.hpp"

class Henchman_3_left: public Sprite {
  // _height = 4
  // _width = 4
 public:
  Henchman_3_left(unsigned height, unsigned width): Sprite(height, width) {}
  ~Henchman_3_left() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
