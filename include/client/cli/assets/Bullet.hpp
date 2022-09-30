#pragma once

#include <ncurses.h>

#include "Sprite.hpp"

class Bullet: public Sprite {
  // _height = 1
  // _width = 1
 public:
  Bullet(unsigned height, unsigned width): Sprite(height, width) {}
  ~Bullet() override = default;

  void draw(WINDOW* win, int y, int x) const override;
};
