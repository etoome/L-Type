#include "Enemy.hpp"

class Enemy_1: public Enemy {
 private:
  // _height = 1
  // _width = 3
 public:
  Enemy_1(unsigned height, unsigned width): Enemy(height, width) {}
  ~Enemy_1() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
