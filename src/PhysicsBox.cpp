#include "PhysicsBox.hpp"

PhysicsBox::PhysicsBox(
    double _xPos,
    double _yPos,
    int _xSize,
    int _ySize,
    double _xVelocity,
    double _yVelocity) noexcept
    : xPos(_xPos),
      yPos(_yPos),
      xSize(_xSize),
      ySize(_ySize),
      xVelocity(_xVelocity),
      yVelocity(_yVelocity) {}
