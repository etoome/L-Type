#pragma once

#include <vector>

#include "GameSettings.hpp"
#include "server/game/Entity.hpp"
#include "EntityInfo.hpp"
#include "server/game/Group.hpp"
#include "server/game/Map.hpp"

class PhysicsEngine {
 private:
  Map* _map;
  Player* _players[2] = {nullptr, nullptr};
  void _checkCollision(Group::Entities::iterator&, Group::Entities::iterator&);

  bool _friendlyFire;
  unsigned _initialLives;
  double _bonusProbability;
  double _difficulty;

 public:
  PhysicsEngine(bool friendlyFire, unsigned initialLives, double bonusProbability, double difficulty) noexcept;
  ~PhysicsEngine() noexcept;
  PhysicsEngine(const PhysicsEngine&) = delete;
  PhysicsEngine& operator=(const PhysicsEngine&) = delete;

  void newEntity(const EntityInfo&);
  void newPlayer(std::size_t nPlayer, const EntityInfo&) noexcept;

  void makeMoves();
  void cleanOffScreen();
  void checkCollisions();

  /* Delete all entities excepted players.
   */
  void clearMap();

  void refreshStates();
  void makeAttacks();
  void resetStates();

  void setPlayerVelocityX(int nPlayer, int direction);
  void setPlayerVelocityY(int nPlayer, int direction);
  void playerShoot(int);

  void getPlayers(const Player*& player1, const Player*& player2) const noexcept;

  std::size_t getEntityNumber() const noexcept;
  std::size_t getEnemyNumber() const noexcept;

  std::vector<Entity*>& getAllEntities(std::vector<Entity*>& dest) const noexcept;

  void playersNewLife() const noexcept;
  void playersToggleGhost() const noexcept;
  void playersToggleHulk() const noexcept;
};
