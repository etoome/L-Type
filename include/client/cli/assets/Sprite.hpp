#pragma once

#include <ncurses.h>

class Sprite {
 public:
  const unsigned _height;
  const unsigned _width;

  Sprite(unsigned l, unsigned w): _height(l), _width(w) {}
  virtual ~Sprite() = default;

  virtual void draw(WINDOW* win, int y, int x) const = 0;
};
