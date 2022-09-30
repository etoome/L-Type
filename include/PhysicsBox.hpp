#pragma once

struct PhysicsBox {
  double xPos;
  double yPos;
  int xSize;
  int ySize;
  double xVelocity;
  double yVelocity;

  PhysicsBox() noexcept = delete;
  PhysicsBox(double xPos, double yPos, int xSize, int ySize, double xVelocity, double yVelocity) noexcept;
};
