#include "client/gui/Activity.hpp"

Activity::Activity(const Mapping& mapping): _userMapping(mapping) {}

void Activity::addEntity(const Entity& entity) {
  _entities.push_back(entity);
}

void Activity::clearEntities() {
  _entities.clear();
}
