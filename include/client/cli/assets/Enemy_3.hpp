#include "Enemy.hpp"

class Enemy_3: public Enemy {
 private:
  // _height = 3
  // _width = 3
 public:
  Enemy_3(unsigned height, unsigned width): Enemy(height, width) {}
  ~Enemy_3() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
