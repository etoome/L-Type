#include "Obstacle.hpp"

class Obstacle_2: public Obstacle {
 private:
  // _height = 5
  // _width = 3
 public:
  Obstacle_2(unsigned height, unsigned width): Obstacle(height, width) {}
  ~Obstacle_2() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
