#include "Obstacle.hpp"

class Obstacle_3: public Obstacle {
 private:
  // _height = 3
  // _width = 3
 public:
  Obstacle_3(unsigned height, unsigned width): Obstacle(height, width) {}
  ~Obstacle_3() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
