#include "server/game/PhysicsEngine.hpp"

#include <utility>

#include "assetsID.hpp"

PhysicsEngine::PhysicsEngine(bool friendlyFire, unsigned initialLives, double bonusProbability, double difficulty) noexcept
    : _map(new Map()),
      _friendlyFire(friendlyFire),
      _initialLives(initialLives),
      _bonusProbability(bonusProbability),
      _difficulty(difficulty) {}

PhysicsEngine::~PhysicsEngine() noexcept {
  delete _map;
}

void PhysicsEngine::newEntity(const EntityInfo& entityInfo) {
  Entity* entity = nullptr;

  switch (entityInfo.mainType()) {
    case ASSET_ENEMY_TYPE:
      switch (entityInfo.fullType()) {
        case ASSET_ENEMY_1_ID:
          entity = new Enemy_1(entityInfo.fullType(), entityInfo.physicsBox(), _map, _bonusProbability, _difficulty);
          break;

        case ASSET_ENEMY_2_ID:
          entity = new Enemy_2(entityInfo.fullType(), entityInfo.physicsBox(), _map, _bonusProbability, _difficulty);
          break;
        case ASSET_ENEMY_3_ID:
          entity = new Enemy_3(entityInfo.fullType(), entityInfo.physicsBox(), _map, _bonusProbability, _difficulty);
          break;
      }
      break;
    case ASSET_OBSTACLE_TYPE:
      entity = new Obstacle(entityInfo.fullType(), entityInfo.physicsBox(), _map);
      break;
    case ASSET_BOSS_TYPE:
      switch (entityInfo.fullType()) {
        case ASSET_BOSS_1_ID:
          entity = new Boss_1(entityInfo.fullType(), entityInfo.physicsBox(), _map, _bonusProbability, _difficulty);
          break;
        case ASSET_BOSS_2_ID:
          entity = new Boss_2(entityInfo.fullType(), entityInfo.physicsBox(), _map, _bonusProbability, _difficulty);
          break;
        case ASSET_BOSS_3_ID:
          entity = new Boss_3(entityInfo.fullType(), entityInfo.physicsBox(), _map, _bonusProbability, _difficulty);
          break;
      }
      break;
  }

  if (entity) {
    _map->add(entity);
  }
}

void PhysicsEngine::newPlayer(std::size_t nPlayer, const EntityInfo& entityInfo) noexcept {
  _players[nPlayer] = new Player(entityInfo.fullType(), entityInfo.physicsBox(), _map, _friendlyFire, _initialLives);
  _map->add(_players[nPlayer]);
}

void PhysicsEngine::refreshStates() {
  for (Group* group: _map->groups()) {
    for (Entity* entity: group->entities()) {
      entity->refreshState();
    }
  }
}

void PhysicsEngine::makeAttacks() {
  for (Entity* entityPtr: _map->group(ENEMY).entities()) {
    if (Enemy* enemyPtr = dynamic_cast<Enemy*>(entityPtr)) {
      enemyPtr->shoot();
    }
  }
}

void PhysicsEngine::resetStates() {
  for (Player* player: _players) {
    if (player) {
      if (player->checkDeath()) {
        player->respawn();
        _map->add(player);
      }

      player->resetState();
    }
  }
}

void PhysicsEngine::makeMoves() {
  for (Group* group: _map->groups()) {
    for (Entity* entityPtr: group->entities()) {
      entityPtr->move();
    }
  }
}

void PhysicsEngine::cleanOffScreen() {
  for (Group* group: _map->groups()) {
    for (size_t e = 0; e != group->size(); ++e) {
      if (_map->isOffMap(group->entity(e))) {
        delete group->entity(e);
        --e;
      }
    }
  }
}

void PhysicsEngine::_checkCollision(Group::Entities::iterator& entityIt1, Group::Entities::iterator& entityIt2) {
  Entity* entity1 = *entityIt1;
  Entity* entity2 = *entityIt2;

  if (entity1->isTouching(entity2)) {
    entity1->touch(entity2);

    if (dynamic_cast<Player*>(entity1) && dynamic_cast<PowerUp*>(entity2)) {
      --entityIt2;
    }
  }

  size_t nEntity = 0;
  for (Entity* entity: {entity1, entity2}) {
    if (PhysicalEntity* pEntity = dynamic_cast<PhysicalEntity*>(entity)) {
      if (pEntity->checkDeath()) {
        pEntity->kill();

        if (nEntity == 0) {
          --entityIt1;
        } else {
          --entityIt2;
        }

        // Delete dead entities except players
        if (!dynamic_cast<Player*>(pEntity)) {
          delete pEntity;
        }
      }
    }
    ++nEntity;
  }
}

void PhysicsEngine::checkCollisions() {
  for (Group* group1: _map->groups()) {
    for (Group* group2: group1->collisionGroups()) {
      Group::Entities::iterator entityIt1 = group1->entities().begin();
      while (entityIt1 != group1->entities().end()) {
        Group::Entities::iterator initialEntityIt1 = entityIt1;

        Group::Entities::iterator entityIt2 = (group1 == group2) ? entityIt1 + 1 : group2->entities().begin();
        while (entityIt2 != group2->entities().end()) {
          _checkCollision(entityIt1, entityIt2);

          // Change entity1 if the previous one was deleted
          if (initialEntityIt1 != entityIt1) {
            break;
          }

          ++entityIt2;
        }

        ++entityIt1;
      }
    }
  }
}

void PhysicsEngine::clearMap() {
  for (size_t g = 1; g != _map->groups().size(); ++g) {
    Group* group = &_map->group(g);
    for (size_t e = 0; e != group->size(); ++e) {
      delete group->entity(e);
      --e;
    }
  }
}

void PhysicsEngine::setPlayerVelocityY(int nPlayer, int direction) {
  double velocity = direction * PLAYER_VELOCITY_Y;
  if (_players[nPlayer] && unsigned(_players[nPlayer]->yPos() + velocity) < MAP_HEIGHT) {
    _players[nPlayer]->setVelocityY(velocity);
  }
}

void PhysicsEngine::setPlayerVelocityX(int nPlayer, int direction) {
  double velocity = direction * PLAYER_VELOCITY_X;
  if (_players[nPlayer] && unsigned(_players[nPlayer]->xPos() + velocity) < MAP_WIDTH) {
    _players[nPlayer]->setVelocityX(velocity);
  }
}

void PhysicsEngine::playerShoot(int nPlayer) {
  if (_players[nPlayer] && _players[nPlayer]->hp() != 0) {
    _players[nPlayer]->shoot();
  }
}

void PhysicsEngine::getPlayers(const Player*& player1, const Player*& player2) const noexcept {
  player1 = _players[0];
  player2 = _players[1];
}

std::size_t PhysicsEngine::getEntityNumber() const noexcept {
  std::size_t nbEntities = 0;
  for (Group* group: _map->groups()) {
    nbEntities += group->size();
  }
  return nbEntities;
}

std::size_t PhysicsEngine::getEnemyNumber() const noexcept {
  return _map->nbEntities(ENEMY);
}

std::vector<Entity*>& PhysicsEngine::getAllEntities(std::vector<Entity*>& dest) const noexcept {
  for (Group* group: _map->groups()) {
    for (Entity* entity: group->entities()) {
      dest.push_back(entity);
    }
  }
  return dest;
}

void PhysicsEngine::playersNewLife() const noexcept {
  _players[0]->addLife();
  if (_players[1]) {
    _players[1]->addLife();
  }
}

void PhysicsEngine::playersToggleGhost() const noexcept {
  _players[0]->toggleGhost();
  if (_players[1]) {
    _players[1]->toggleGhost();
  }
}

void PhysicsEngine::playersToggleHulk() const noexcept {
  _players[0]->toggleHulk();
  if (_players[1]) {
    _players[1]->toggleHulk();
  }
}
