#pragma once

#include <ncurses.h>

#include "Sprite.hpp"

class Obstacle: public Sprite {
 public:
  Obstacle(unsigned height, unsigned width): Sprite(height, width) {}
  ~Obstacle() override = default;
};
