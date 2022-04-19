#pragma once

#include <cstddef>
#include <vector>

#include "server/game/Entity.hpp"

constexpr std::ptrdiff_t PLAYER = 0;
constexpr std::ptrdiff_t ENEMY = 1;
constexpr std::ptrdiff_t BULLET = 2;
constexpr std::ptrdiff_t OBSTACLE = 3;
constexpr std::ptrdiff_t POWERUP = 4;

class Entity;
class Group {
 public:
  using Entities = std::vector<Entity*>;
  using Groups = std::vector<Group*>;

 private:
  Entities _entities = {};
  Groups _collisionGroups = {};

 public:
  Group() noexcept = default;
  ~Group() noexcept;

  Entities& entities();
  Entity* entity(std::size_t);
  std::size_t size() const;

  void addEntity(Entity*);
  void removeEntity(Entity*);

  Groups& collisionGroups();
  void addCollisionGroup(Group*) noexcept;
};
