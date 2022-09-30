#pragma once

#include <vector>

#include "assetsID.hpp"
#include "server/game/Group.hpp"
#include "server/game/Map.hpp"
#include "PhysicsBox.hpp"
#include "constants.hpp"

class Group;
class Map;
class Boss;
class Henchman;

class Entity {
 private:
  unsigned _ID;
  PhysicsBox _physicsBox;

  void _moveX() noexcept;
  void _moveY() noexcept;

 protected:
  Map* _map;
  Group* _group;

  unsigned _state = MOVE_STATE;
  unsigned _stateStep = 0;

  void setxPos(double) noexcept;
  void setyPos(double) noexcept;
  void resetVelocity();

 public:
  Entity(unsigned, const PhysicsBox&, Map*, Group*) noexcept;
  virtual ~Entity() noexcept;
  Entity(const Entity&) = delete;
  Entity& operator=(const Entity&) = delete;

  unsigned ID() const noexcept;
  Group* group() noexcept;
  virtual unsigned state() const noexcept;
  unsigned stateStep() const noexcept;

  double xPos() const noexcept;
  double yPos() const noexcept;
  int xSize() const noexcept;
  int ySize() const noexcept;
  double xVelocity() const noexcept;
  double yVelocity() const noexcept;

  void setVelocityX(double);
  void setVelocityY(double);

  virtual void move();
  virtual void refreshState() noexcept;

  void removeFromGroup();

  bool isTouching(const Entity*) const noexcept;
  virtual void touch(Entity*) noexcept = 0;
};

class PowerUp: public Entity {
 private:
  double _fireDamageFactor;
  double _fireRateFactor;

 public:
  PowerUp(unsigned, const PhysicsBox&, Map*, double additionnalDamage, double additionnalFireRate) noexcept;
  ~PowerUp() noexcept override = default;

  double fireDamageFactor() const noexcept;
  double fireRateFactor() const noexcept;

  void touch(Entity*) noexcept override {}
};

class PhysicalEntity: public Entity {
 protected:
  double _hp;
  double _damage;

 public:
  PhysicalEntity(unsigned, const PhysicsBox&, Map*, Group*, double hp, double damage) noexcept;
  ~PhysicalEntity() noexcept override = 0;

  double getDamage() const noexcept;
  double hp() const noexcept;

  virtual bool checkDeath() noexcept;

  virtual void hurt(double damage) noexcept;
  void touch(Entity*) noexcept override;
  virtual void kill() noexcept;
};

class Obstacle: public PhysicalEntity {
 public:
  Obstacle() = delete;
  ~Obstacle() noexcept override = default;
  Obstacle(unsigned, const PhysicsBox&, Map*) noexcept;
};

class Player;
class Bullet: public PhysicalEntity {
 private:
  Player* _shooter;

 public:
  Bullet(unsigned, const PhysicsBox&, Map*, double fireDamage, Player* shooter = nullptr) noexcept;
  ~Bullet() noexcept override = default;
  Bullet(const Bullet&) = delete;
  Bullet& operator=(const Bullet&) = delete;

  Player* getShooter() const noexcept;
};

class Character: public PhysicalEntity {
 protected:
  double _fireDamage;
  unsigned _fireDelay;
  unsigned _remainingDelay = 0;

  virtual Bullet* _createBullet(double xOffset, double xVelocity = 0) noexcept = 0;

 public:
  Character(unsigned, const PhysicsBox&, Map*, Group*, double hp, double damage, double fireDamage, unsigned fireDelay) noexcept;
  ~Character() noexcept override = default;

  virtual double fireDamage() const noexcept;
  virtual unsigned fireDelay() const noexcept;

  bool checkDeath() noexcept override;

  virtual void shoot() noexcept;
};

class Enemy: public Character {
 private:
  std::size_t _counter = 0;

  PowerUp* _dropPowerUp() const noexcept;

 protected:
  Bullet* _createBullet(double xOffset, double xVelocity = 0) noexcept override;

  const double _bonusProbability;
  const double _difficulty;

 public:
  Enemy(unsigned, const PhysicsBox&, Map*, double hp, unsigned fireDelay, double fireDamage, double bonusProbability, double difficulty) noexcept;
  ~Enemy() noexcept override = default;

  void touch(Entity*) noexcept override;
  void kill() noexcept override;

  void move() override;
  virtual const std::vector<std::array<int, 2>>& getMoves() const = 0;
};

class Enemy_1: public Enemy {
 private:
  static const std::vector<std::array<int, 2>> _moves;

 public:
  Enemy_1(unsigned, const PhysicsBox&, Map*, double bonusProbability, double difficulty) noexcept;
  ~Enemy_1() noexcept override = default;

  const std::vector<std::array<int, 2>>& getMoves() const override;
};

class Enemy_2: public Enemy {
 private:
  static const std::vector<std::array<int, 2>> _moves;

 public:
  Enemy_2(unsigned, const PhysicsBox&, Map*, double bonusProbability, double difficulty) noexcept;
  ~Enemy_2() noexcept override = default;

  const std::vector<std::array<int, 2>>& getMoves() const override;
};

class Enemy_3: public Enemy {
 private:
  static const std::vector<std::array<int, 2>> _moves;

 public:
  Enemy_3(unsigned, const PhysicsBox&, Map*, double bonusProbability, double difficulty) noexcept;
  ~Enemy_3() noexcept override = default;

  const std::vector<std::array<int, 2>>& getMoves() const override;
};

class Player: public Character {
 private:
  double _spawnX;
  double _spawnY;
  unsigned _score = 0;
  unsigned _invincibilityTime = 0;

  bool _ghost = false;
  bool _hulk = false;

  PowerUp* _powerUp = nullptr;

 protected:
  const bool _friendlyFire;
  const unsigned _initialLives;

  Bullet* _createBullet(double xOffset, double xVelocity = 0) noexcept override;

 public:
  Player(unsigned, const PhysicsBox&, Map*, bool friendlyFire, unsigned initialLives) noexcept;
  ~Player() noexcept = default;
  Player(const Player&) = delete;
  Player& operator=(const Player&) = delete;

  double fireDamage() const noexcept override;
  unsigned fireDelay() const noexcept override;
  unsigned score() const noexcept;
  unsigned powerUpID() const noexcept;
  unsigned state() const noexcept override;

  void incScore(unsigned incValue) noexcept;

  void refreshState() noexcept override;
  void resetState() noexcept;

  void hurt(double damage) noexcept override;
  void pick(PowerUp*) noexcept;
  void touch(Entity*) noexcept override;
  void respawn() noexcept;

  void addLife() noexcept;
  void toggleGhost() noexcept;
  void toggleHulk() noexcept;
};


/**********************************************************************
 *                                BOSS                                *
 **********************************************************************/

class Boss: public Enemy {
 private:
  const static std::vector<std::array<int, 2>> _movesBefore;  // Before killing henchmen
  const static std::vector<std::array<int, 2>> _movesAfter;   // After killed henchmen

  bool _gatlingMode = false;

  virtual Henchman* _createHenchman_1(int xPos, int henchmanWidth, int henchmanHeight) noexcept = 0;
  virtual Henchman* _createHenchman_2(int xPos, int henchmanWidth, int henchmanHeight) noexcept = 0;

 protected:
  unsigned _remainingDelay2;

 public:
  Boss(unsigned ID, const PhysicsBox& physicsBox, Map* map, double hp, unsigned fireDelay, double bonusProbability, double difficulty) noexcept;
  ~Boss() noexcept override = default;

  const std::vector<std::array<int, 2>>& getMoves() const override;
  void hurt(double damage) noexcept override;
  virtual void shoot() noexcept override;

  virtual void spawnHenchmen(int henchmanWidth, int henchmanHeight, unsigned nHenchman) noexcept;
};

class Boss_1 final: public Boss {
 private:
  Henchman* _createHenchman_1(int xPos, int henchmanWidth, int henchmanHeight) noexcept override;
  Henchman* _createHenchman_2(int xPos, int henchmanWidth, int henchmanHeight) noexcept override;

 public:
  Boss_1(unsigned ID, const PhysicsBox& physicsBox, Map* map, double bonusProbability, double difficulty) noexcept;
  ~Boss_1() noexcept override = default;
};

class Boss_2 final: public Boss {
 private:
  Henchman* _createHenchman_1(int xPos, int henchmanWidth, int henchmanHeight) noexcept override;
  Henchman* _createHenchman_2(int xPos, int henchmanWidth, int henchmanHeight) noexcept override;

 public:
  Boss_2(unsigned ID, const PhysicsBox& physicsBox, Map* map, double bonusProbability, double difficulty) noexcept;
  ~Boss_2() noexcept override = default;
  void spawnHenchmen(int henchmanWidth, int henchmanHeight, unsigned nHenchman) noexcept override;
};

class Boss_3 final: public Boss {
 private:
  Henchman* _createHenchman_1(int xPos, int henchmanWidth, int henchmanHeight) noexcept override;
  Henchman* _createHenchman_2(int xPos, int henchmanWidth, int henchmanHeight) noexcept override;

 public:
  Boss_3(unsigned ID, const PhysicsBox& physicsBox, Map* map, double bonusProbability, double difficulty) noexcept;
  ~Boss_3() noexcept override = default;
  void spawnHenchmen(int henchmanWidth, int henchmanHeight, unsigned nHenchman) noexcept override;
};

class Henchman: public Enemy {
 private:
  const static std::vector<std::array<int, 2>> _moves;

  Boss* _creator;

 public:
  Henchman(unsigned ID, const PhysicsBox& physicsBox, Map* map, Boss* creator, double hp, unsigned fireDelay, double bonusProbability, double difficulty) noexcept;
  ~Henchman() noexcept override = default;
  Henchman(const Henchman&) = delete;
  Henchman& operator=(const Henchman&) = delete;

  const std::vector<std::array<int, 2>>& getMoves() const override;
  void shoot() noexcept override;
};

// Tentacles
class Henchman_1: public Henchman {
 public:
  Henchman_1(unsigned ID, const PhysicsBox& physicsBox, Map* map, Boss* creator, double bonusProbability, double difficulty) noexcept;
  ~Henchman_1() noexcept override = default;
};

// Little space invaders
class Henchman_2: public Henchman {
 public:
  Henchman_2(unsigned ID, const PhysicsBox& physicsBox, Map* map, Boss* creator, double bonusProbability, double difficulty) noexcept;
  ~Henchman_2() noexcept override = default;
};

// Yooda
class Henchman_3: public Henchman {
 public:
  Henchman_3(unsigned ID, const PhysicsBox& physicsBox, Map* map, Boss* creator, double bonusProbability, double difficulty) noexcept;
  ~Henchman_3() noexcept override = default;
};
