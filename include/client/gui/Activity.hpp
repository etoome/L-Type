#pragma once

#include <array>
#include <vector>

#include "client/gui/assets/Sprite.hpp"
#include "constants.hpp"

class Activity {
 public:
  using Mapping = std::array<std::size_t, MAX_PLAYERS>;
  struct Entity {
    Sprite sprite;
    int xPos;
    int yPos;

    Entity(const Sprite& s, int x, int y): sprite(s), xPos(x), yPos(y) {}
    Entity(const Entity& other): sprite(other.sprite), xPos(other.xPos), yPos(other.yPos) {}
    Entity& operator=(Entity&& other) {
      sprite = other.sprite;
      xPos = other.xPos;
      yPos = other.yPos;
      return *this;
    }
  };

 protected:
  std::vector<Entity> _entities = {};
  const std::array<std::size_t, MAX_PLAYERS> _userMapping;

 public:
  Activity(const Mapping& mapping);
  virtual ~Activity() = default;

  void addEntity(const Entity&);
  void clearEntities();

};
