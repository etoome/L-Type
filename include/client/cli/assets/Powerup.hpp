#pragma once

#include <ncurses.h>

#include "Sprite.hpp"

class Powerup: public Sprite {
  // _height = 1
  // _width = 1
 public:
  Powerup(unsigned height, unsigned width): Sprite(height, width) {}
  ~Powerup() override = default;
};
