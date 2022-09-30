#pragma once

#include <ncurses.h>

#include "Sprite.hpp"

class Henchman_2_right: public Sprite {
  // _height = 3
  // _width = 3
 public:
  Henchman_2_right(unsigned height, unsigned width): Sprite(height, width) {}
  ~Henchman_2_right() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
