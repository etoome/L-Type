#pragma once

#include <string>

class Sprite {
 public:
  unsigned id;
  int width;
  int height;
  std::string path;

  Sprite(unsigned i, int w, int h, std::string p): id(i), width(w), height(h), path(p) {}
  ~Sprite() = default;
};
