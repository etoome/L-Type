#pragma once

#include <ncurses.h>

#include "Sprite.hpp"

class Player_1: public Sprite {
  // _height = 2
  // _width = 2
 public:
  Player_1(unsigned height, unsigned width): Sprite(height, width) {}
  ~Player_1() override = default;

  void draw(WINDOW* win, int y, int x) const override;
};
