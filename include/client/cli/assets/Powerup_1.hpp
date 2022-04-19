#include "Powerup.hpp"

class Powerup_1: public Powerup {
 private:
  // _height = 1
  // _width = 1
 public:
  Powerup_1(unsigned height, unsigned width): Powerup(height, width) {}
  ~Powerup_1() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
