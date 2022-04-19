#pragma once

#include <ncurses.h>

#include "Sprite.hpp"

class Boss_1: public Sprite {
  // _height = 6
  // _width = 6
 public:
  Boss_1(unsigned height, unsigned width): Sprite(height, width) {}
  ~Boss_1() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
