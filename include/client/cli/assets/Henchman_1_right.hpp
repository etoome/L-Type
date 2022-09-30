#pragma once

#include <ncurses.h>

#include "Sprite.hpp"

class Henchman_1_right: public Sprite {
  // _height = 6
  // _width = 8
 public:
  Henchman_1_right(unsigned height, unsigned width): Sprite(height, width) {}
  ~Henchman_1_right() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
