#include "server/game/Group.hpp"

#include <algorithm>

Group::~Group() noexcept {
  Entities::iterator entityIt = _entities.begin();
  while (entityIt != _entities.end()) {
    delete *entityIt;
  }
}

Group::Entities& Group::entities() {
  return _entities;
}

Entity* Group::entity(std::size_t idx) {
  return _entities.at(idx);
}

std::size_t Group::size() const {
  return _entities.size();
}

Group::Groups& Group::collisionGroups() {
  return _collisionGroups;
}

void Group::addCollisionGroup(Group* group) noexcept {
  _collisionGroups.push_back(group);
}

void Group::addEntity(Entity* entity) {
  _entities.push_back(entity);
}

void Group::removeEntity(Entity* entity) {
  _entities.erase(std::remove(_entities.begin(), _entities.end(), entity));
}
