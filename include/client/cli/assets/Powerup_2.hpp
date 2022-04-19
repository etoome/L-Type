#include "Powerup.hpp"

class Powerup_2: public Powerup {
 private:
  // _height = 1
  // _width = 1
 public:
  Powerup_2(unsigned height, unsigned width): Powerup(height, width) {}
  ~Powerup_2() = default;

  void draw(WINDOW* win, int y, int x) const override;
};
