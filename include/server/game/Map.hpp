#pragma once

#include <array>
#include <cstddef>

#include "constants.hpp"
#include "server/game/Entity.hpp"
#include "server/game/Group.hpp"

class Entity;
class Group;

class Map {
 private:
  static constexpr size_t NB_GROUPS = 5;

 public:
  using Groups = std::array<Group*, NB_GROUPS>;

 private:
  std::array<Group*, NB_GROUPS> _groups = {};

  void _setCollisionGroups() noexcept;

 public:
  Map() noexcept;
  ~Map() noexcept;

  Groups& groups();
  Group& group(std::size_t nGroup);

  void add(Entity*);
  void remove(Entity*);
  std::size_t nbEntities(std::size_t nGroup) const noexcept;

  bool isOffMap(const Entity*) const noexcept;
};
