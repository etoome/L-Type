#include "Enemy.hpp"

class Enemy_2: public Enemy {
 private:
  // _height = 2
  // _width = 3
 public:
  Enemy_2(unsigned height, unsigned width): Enemy(height, width) {}
  ~Enemy_2() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
