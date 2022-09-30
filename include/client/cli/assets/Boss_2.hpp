#pragma once

#include <ncurses.h>

#include "Sprite.hpp"

class Boss_2: public Sprite {
  // _height = 8
  // _width = 20
 public:
  Boss_2(unsigned height, unsigned width): Sprite(height, width) {}
  ~Boss_2() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
