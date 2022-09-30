#include "server/game/Map.hpp"

Map::Map() noexcept {
  for (size_t g = 0; g < NB_GROUPS; ++g) {
    _groups[g] = new Group();
  }

  _setCollisionGroups();
}

Map::~Map() noexcept {
  for (Group* g: _groups) {
    delete g;
  }
}

inline void Map::_setCollisionGroups() noexcept {
  _groups[PLAYER]->addCollisionGroup(_groups[PLAYER]);
  _groups[PLAYER]->addCollisionGroup(_groups[ENEMY]);
  _groups[PLAYER]->addCollisionGroup(_groups[BULLET]);
  _groups[PLAYER]->addCollisionGroup(_groups[OBSTACLE]);
  _groups[PLAYER]->addCollisionGroup(_groups[POWERUP]);

  _groups[ENEMY]->addCollisionGroup(_groups[BULLET]);

  _groups[BULLET]->addCollisionGroup(_groups[BULLET]);
  _groups[BULLET]->addCollisionGroup(_groups[OBSTACLE]);
}

Map::Groups& Map::groups() {
  return _groups;
}

Group& Map::group(std::size_t nGroup) {
  return *_groups.at(nGroup);
}

void Map::add(Entity* entity) {
  entity->group()->addEntity(entity);
}

void Map::remove(Entity* entity) {
  entity->removeFromGroup();
}

std::size_t Map::nbEntities(std::size_t nGroup) const noexcept {
  return _groups.at(nGroup)->size();
}

bool Map::isOffMap(const Entity* entity) const noexcept {
  return !(0 < entity->xPos() + entity->xSize() &&
           MAP_WIDTH > entity->xPos() &&
           0 < entity->yPos() + entity->ySize() &&
           MAP_HEIGHT > entity->yPos());
}
