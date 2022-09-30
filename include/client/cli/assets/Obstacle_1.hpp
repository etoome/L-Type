#include "Obstacle.hpp"

class Obstacle_1: public Obstacle {
 private:
  // _height = 4
  // _width = 3
 public:
  Obstacle_1(unsigned height, unsigned width): Obstacle(height, width) {}
  ~Obstacle_1() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
