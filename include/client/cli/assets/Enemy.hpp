#pragma once

#include <ncurses.h>

#include "Sprite.hpp"

class Enemy: public Sprite {
  // _height = 3
  // _width = 3
 public:
  Enemy(unsigned height, unsigned width): Sprite(height, width) {}
  ~Enemy() override = default;
};
