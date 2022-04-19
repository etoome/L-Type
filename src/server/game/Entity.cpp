#include "server/game/Entity.hpp"

#include <array>
#include <cmath>

#include "random"
#include "utils.hpp"

/**********************************************************************
 *                               ENTITY                               *
 **********************************************************************/

Entity::Entity(unsigned ID, const PhysicsBox& physicsBox, Map* map, Group* group) noexcept
    : _ID(ID), _physicsBox(physicsBox), _map(map), _group(group) {}

Entity::~Entity() noexcept {
  removeFromGroup();
}

inline double Entity::xPos() const noexcept { return _physicsBox.xPos; }
inline double Entity::yPos() const noexcept { return _physicsBox.yPos; }
inline int Entity::xSize() const noexcept { return _physicsBox.xSize; }
inline int Entity::ySize() const noexcept { return _physicsBox.ySize; }
unsigned Entity::ID() const noexcept { return _ID; }

void Entity::setxPos(double xPos) noexcept { _physicsBox.xPos = xPos; }
void Entity::setyPos(double yPos) noexcept { _physicsBox.yPos = yPos; }

double Entity::xVelocity() const noexcept { return _physicsBox.xVelocity; }
double Entity::yVelocity() const noexcept { return _physicsBox.yVelocity; }

void Entity::setVelocityX(double velocity) {
  _physicsBox.xVelocity = velocity;
}

void Entity::setVelocityY(double velocity) {
  _physicsBox.yVelocity = velocity;
}

void Entity::resetVelocity() {
  _physicsBox.xVelocity = 0;
  _physicsBox.yVelocity = 0;
}

void Entity::refreshState() noexcept {
  ++_stateStep %= STATE_DURATION;
  if (_stateStep == 0) {
    _state = MOVE_STATE;
  }
}

unsigned Entity::state() const noexcept {
  return _state;
}

unsigned Entity::stateStep() const noexcept {
  return _stateStep;
}

Group* Entity::group() noexcept { return _group; }

void Entity::_moveX() noexcept {
  _physicsBox.xPos += _physicsBox.xVelocity;
}

void Entity::_moveY() noexcept {
  _physicsBox.yPos += _physicsBox.yVelocity;
}

void Entity::move() {
  _moveX();
  _moveY();
}

bool Entity::isTouching(const Entity* other) const noexcept {
  if (_state == DIE_STATE || other->state() == DIE_STATE) return false;
  return (xPos() < other->xPos() + other->xSize() &&
          xPos() + xSize() > other->xPos() &&
          yPos() < other->yPos() + other->ySize() &&
          yPos() + ySize() > other->yPos());
}

void Entity::removeFromGroup() {
  if (_group) {
    _group->removeEntity(this);
    _group = nullptr;
  }
}

/**********************************************************************
 *                              POWERUP                               *
 **********************************************************************/

PowerUp::PowerUp(unsigned ID, const PhysicsBox& physicsBox, Map* map, double fireDamageFactor, double fireRateFactor) noexcept
    : Entity(ID, physicsBox, map, &map->group(POWERUP)),
      _fireDamageFactor(fireDamageFactor),
      _fireRateFactor(fireRateFactor) {}

double PowerUp::fireDamageFactor() const noexcept {
  return _fireDamageFactor;
}

double PowerUp::fireRateFactor() const noexcept {
  return _fireRateFactor;
}

/**********************************************************************
 *                           PHYSICALENTITY                           *
 **********************************************************************/

PhysicalEntity::PhysicalEntity(unsigned ID, const PhysicsBox& physicsBox, Map* map, Group* group, double hp, double damage) noexcept
    : Entity(ID, physicsBox, map, group), _hp(hp), _damage(damage) {}

PhysicalEntity::~PhysicalEntity() noexcept {}

double PhysicalEntity::getDamage() const noexcept {
  return _damage;
}

double PhysicalEntity::hp() const noexcept {
  return _hp;
}

bool PhysicalEntity::checkDeath() noexcept {
  return _hp <= 0;
}

void PhysicalEntity::hurt(double damage) noexcept {
  if (_hp == 0) return;

  double floatPart = _hp - floor(_hp);
  if (floatPart == 0) {
    floatPart = 1.0;
  }
  _hp -= (damage < floatPart) ? damage : floatPart;

  _state = HURT_STATE;
  _stateStep = 0;
}

void PhysicalEntity::touch(Entity* other) noexcept {
  if (PhysicalEntity* entity = dynamic_cast<PhysicalEntity*>(other)) {
    hurt(entity->getDamage());
    entity->hurt(getDamage());
  }
}

void PhysicalEntity::kill() noexcept {
  removeFromGroup();
}

/**********************************************************************
 *                              OBSTACLE                              *
 **********************************************************************/

Obstacle::Obstacle(unsigned ID, const PhysicsBox& physicsBox, Map* map) noexcept
    : PhysicalEntity(ID, physicsBox, map, &map->group(OBSTACLE), OBSTACLE_HP, OBSTACLE_DAMAGE) {
  setVelocityY(OBSTACLE_VELOCITY);
}

/**********************************************************************
 *                               BULLET                               *
 **********************************************************************/

Bullet::Bullet(unsigned ID, const PhysicsBox& physicsBox, Map* map, double fireDamage, Player* shooter) noexcept
    : PhysicalEntity(ID, physicsBox, map, &map->group(BULLET), BULLET_HP, fireDamage), _shooter(shooter) {}

Player* Bullet::getShooter() const noexcept {
  return _shooter;
}

/**********************************************************************
 *                             CHARACTER                              *
 **********************************************************************/

Character::Character(unsigned ID, const PhysicsBox& physicsBox, Map* map, Group* group, double hp, double damage, double fireDamage, unsigned fireDelay) noexcept
    : PhysicalEntity(ID, physicsBox, map, group, hp, damage), _fireDamage(fireDamage), _fireDelay(fireDelay) {}

double Character::fireDamage() const noexcept {
  return _fireDamage;
}

unsigned Character::fireDelay() const noexcept {
  return _fireDelay;
}

void Character::shoot() noexcept {
  if (_remainingDelay == 0 && _state != DIE_STATE) {
    _map->add(_createBullet(xSize() / 2));
    _remainingDelay = fireDelay();

    _state = SHOOT_STATE;
    _stateStep = 0;
  } else {
    --_remainingDelay;
  }
}

bool Character::checkDeath() noexcept {
  if (!PhysicalEntity::checkDeath()) return false;

  bool died = false;
  if (_state != DIE_STATE) {
    _state = DIE_STATE;
    _stateStep = 0;
  } else if (_stateStep == STATE_DURATION - 1) {
    died = true;
  }
  return died;
}

/**********************************************************************
 *                               ENNEMY                               *
 **********************************************************************/

Enemy::Enemy(unsigned ID, const PhysicsBox& physicsBox, Map* map, double hp, unsigned fireDelay, double fireDamage, double bonusProbability, double difficulty) noexcept
    : Character(ID, physicsBox, map, &map->group(ENEMY), hp, ENEMY_DAMAGE, fireDamage * difficulty, fireDelay),
      _bonusProbability(bonusProbability),
      _difficulty(difficulty) {}

Bullet* Enemy::_createBullet(double xOffset, double xVelocity) noexcept {
  PhysicsBox physicsBox(
      // Spawn the bullet ahead of the character
      xPos() + xOffset + ASSET_BULLET_WIDTH / 2, yPos() + ySize() + ASSET_BULLET_HEIGHT / 2,
      ASSET_BULLET_WIDTH, ASSET_BULLET_HEIGHT,
      xVelocity, BULLET_VELOCITY);
  return new Bullet(ASSET_BULLET_ID, physicsBox, _map, fireDamage());
}

void Enemy::touch(Entity* other) noexcept {
  if (Bullet* bullet = dynamic_cast<Bullet*>(other)) {
    // If the bullet comes from a player
    if (Player* shooter = bullet->getShooter()) {
      PhysicalEntity::touch(other);

      if (checkDeath()) {
        shooter->incScore(SCORE_KILL_ENEMY);
      } else {
        shooter->incScore(SCORE_TOUCH_ENEMY);
      }
    }
  } else {
    PhysicalEntity::touch(other);
  }
}

void Enemy::move() {
  const std::vector<std::array<int, 2>> moves = getMoves();

  setVelocityX(moves[_counter][0] * ENEMY_VELOCITY_X);
  setVelocityY(moves[_counter][1] * ENEMY_VELOCITY_Y);
  Entity::move();

  ++_counter;
  _counter %= moves.size();
}

PowerUp* Enemy::_dropPowerUp() const noexcept {
  PhysicsBox physicsBox(
      xPos() + (xSize() - ASSET_POWERUP_1_WIDTH) / 2, yPos() + (ySize() + ASSET_POWERUP_1_HEIGHT) / 2,
      ASSET_POWERUP_1_WIDTH, ASSET_POWERUP_1_HEIGHT,
      0, 0);

  PowerUp* powerUp;
  if (genRandomDouble(0, 2) <= 1) {
    powerUp = new PowerUp(ASSET_POWERUP_1_ID, physicsBox, _map, POWERUP_DAMAGE_RATE, 1);
  } else {
    powerUp = new PowerUp(ASSET_POWERUP_2_ID, physicsBox, _map, 1, POWERUP_FIRE_RATE);
  }

  return powerUp;
}

void Enemy::kill() noexcept {
  PhysicalEntity::kill();

  if (genRandomDouble(0, 1) <= _bonusProbability) {
    _map->add(_dropPowerUp());
  }
}

/**********************************************************************
 *                               ENEMY VARIANTS                       *
 **********************************************************************/

const std::vector<std::array<int, 2>> Enemy_1::_moves = {
    {1, 1},
    {1, 1},
    {1, 1},
    {1, 1},
    {1, 0},
    {1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 1},
    {-1, 1},
    {-1, 1},
    {-1, 1},
    {-1, 1},
    {-1, 1},
    {-1, 1},
    {-1, 0},
    {-1, 0},
    {1, 0},
    {1, 0},
    {1, 1},
    {1, 1},
    {1, 1},
    {1, 1},
};

Enemy_1::Enemy_1(unsigned ID, const PhysicsBox& physicsBox, Map* map, double bonusProbability, double difficulty) noexcept
    : Enemy(ID, physicsBox, map, ENEMY_HP, ENEMY_FIRE_DELAY, ENEMY_MAX_FIRE_DAMAGE, bonusProbability, difficulty) {}

const std::vector<std::array<int, 2>>& Enemy_1::getMoves() const {
  return Enemy_1::_moves;
}

const std::vector<std::array<int, 2>> Enemy_2::_moves = {
    {0, 1},
};

Enemy_2::Enemy_2(unsigned ID, const PhysicsBox& physicsBox, Map* map, double bonusProbability, double difficulty) noexcept
    : Enemy(ID, physicsBox, map, ENEMY_HP, ENEMY_FIRE_DELAY, ENEMY_MAX_FIRE_DAMAGE, bonusProbability, difficulty) {}

const std::vector<std::array<int, 2>>& Enemy_2::getMoves() const {
  return Enemy_2::_moves;
}

const std::vector<std::array<int, 2>> Enemy_3::_moves = {
    {0, 1},
    {0, 1},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {0, -1},
    {0, -1},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
};

Enemy_3::Enemy_3(unsigned ID, const PhysicsBox& physicsBox, Map* map, double bonusProbability, double difficulty) noexcept
    : Enemy(ID, physicsBox, map, ENEMY_HP, ENEMY_FIRE_DELAY, ENEMY_MAX_FIRE_DAMAGE, bonusProbability, difficulty) {}

const std::vector<std::array<int, 2>>& Enemy_3::getMoves() const {
  return Enemy_3::_moves;
}

/**********************************************************************
 *                               PLAYER                               *
 **********************************************************************/

Player::Player(unsigned ID, const PhysicsBox& physicsBox, Map* map, bool friendlyFire, unsigned initialLives) noexcept
    : Character(ID, physicsBox, map, &map->group(PLAYER), initialLives, PLAYER_DAMAGE, PLAYER_FIRE_DAMAGE, PLAYER_FIRE_DELAY),
      _spawnX(physicsBox.xPos),
      _spawnY(physicsBox.yPos),
      _friendlyFire(friendlyFire),
      _initialLives(initialLives) {}

Bullet* Player::_createBullet(double xOffset, double xVelocity) noexcept {
  PhysicsBox physicsBox(
      // Spawn the bullet ahead of the character
      xPos() + xOffset + ASSET_BULLET_WIDTH / 2, yPos(),
      ASSET_BULLET_WIDTH, ASSET_BULLET_HEIGHT,
      xVelocity, -BULLET_VELOCITY);
  return new Bullet(ASSET_BULLET_ID, physicsBox, _map, fireDamage(), this);
}

double Player::fireDamage() const noexcept {
  double fireDamage = _fireDamage;
  if (_powerUp) {
    fireDamage *= unsigned(_powerUp->fireDamageFactor());
  }
  return fireDamage;
}

unsigned Player::fireDelay() const noexcept {
  unsigned fireDelay = _fireDelay;
  if (_powerUp) {
    fireDelay *= _powerUp->fireRateFactor();
  }
  return fireDelay;
}

unsigned int Player::score() const noexcept {
  return _score;
}

unsigned Player::powerUpID() const noexcept {
  if (_powerUp) {
    return _powerUp->ID();
  } else {
    return 0;
  }
}

unsigned Player::state() const noexcept {
  return (_invincibilityTime == 0) ? Entity::state() : RESPAWN_STATE;
}

void Player::incScore(unsigned int incValue) noexcept {
  _score += incValue;
}

void Player::refreshState() noexcept {
  Entity::refreshState();

  resetVelocity();
  if (_invincibilityTime) {
    --_invincibilityTime;
  }
  if (_remainingDelay) {
    --_remainingDelay;
  }
}

void Player::resetState() noexcept {
  if (_hp != ceil(_hp)) {
    _hp = ceil(_hp);
  } else if (_hp != _initialLives) {
    ++_hp;
  }

  respawn();
}

void Player::pick(PowerUp* powerUp) noexcept {
  delete _powerUp;
  _powerUp = powerUp;
  powerUp->removeFromGroup();

  _state = PICK_POWER_UP_STATE;
  _stateStep = 0;
}

void Player::hurt(double damage) noexcept {
  if (!_ghost && !_hulk) {
    PhysicalEntity::hurt(damage);
    if (_hp == floor(_hp) && !checkDeath()) {
      respawn();
    }
  }
}

void Player::touch(Entity* other) noexcept {
  if (PowerUp* powerUp = dynamic_cast<PowerUp*>(other)) {
    pick(powerUp);
  } else if (!_invincibilityTime && !_ghost) {
    Player* player2 = dynamic_cast<Player*>(other);
    Bullet* bullet = dynamic_cast<Bullet*>(other);

    if (bullet) {
      // If the bullet comes from an ennemy or friendly fire is enabled
      if (!bullet->getShooter() || _friendlyFire) {
        PhysicalEntity::touch(other);
      }
    } else if (player2) {
      // If the bullet comes from an ennemy or friendly fire is enabled
      if (_friendlyFire) {
        PhysicalEntity::touch(player2);
      }
    } else {
      PhysicalEntity::touch(other);
    }
  }
}

void Player::respawn() noexcept {
  delete _powerUp;
  _powerUp = nullptr;

  _group = &_map->group(PLAYER);
  _invincibilityTime = RESPAWN_DURATION;
  setxPos(_spawnX);
  setyPos(_spawnY);
}

void Player::addLife() noexcept {
  _hp += 1;
}

void Player::toggleGhost() noexcept {
  _ghost = !_ghost;
}

void Player::toggleHulk() noexcept {
  _hulk = !_hulk;
}

/**********************************************************************
 *                               BOSS                                 *
 **********************************************************************/

const std::vector<std::array<int, 2>> Boss::_movesBefore = {{0, 0}};
const std::vector<std::array<int, 2>> Boss::_movesAfter = {
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
};

Boss::Boss(unsigned ID, const PhysicsBox& physicsBox, Map* map, double hp, unsigned fireDelay, double bonusProbability, double difficulty) noexcept
    : Enemy(ID, physicsBox, map, hp, fireDelay, ENEMY_MAX_FIRE_DAMAGE, bonusProbability, difficulty),
      _remainingDelay2(fireDelay / 3) {}

const std::vector<std::array<int, 2>>& Boss::getMoves() const {
  if (_map->nbEntities(ENEMY) != 1) {
    return _movesBefore;
  } else {
    return _movesAfter;
  }
}

void Boss::hurt(double damage) noexcept {
  if (_map->nbEntities(ENEMY) == 1) {
    _gatlingMode = true;
    PhysicalEntity::hurt(damage);
  }
}

void Boss::shoot() noexcept {
  unsigned fireDelay = _fireDelay;
  if (_gatlingMode) {
    fireDelay = unsigned(_fireDelay * 0.25);
  }

  if (_remainingDelay == 0) {
    _map->add(_createBullet(xSize() / 3));
    _remainingDelay = fireDelay;
  } else {
    --_remainingDelay;
  }

  if (_remainingDelay2 == 0) {
    _map->add(_createBullet(2 * xSize() / 3));
    _remainingDelay2 = fireDelay;
  } else {
    --_remainingDelay2;
  }
}

void Boss::spawnHenchmen(int henchmanWidth, int henchmanHeight, unsigned nHenchman) noexcept {
  int i = 0;
  int mid = int(nHenchman) / 2;

  int gap = (MAP_WIDTH / (int(nHenchman))) - henchmanWidth;
  int x = MAP_WIDTH / 2 - (mid * henchmanWidth + (mid - 1) * gap);

  while (i != int(nHenchman) + 1 && x != int(nHenchman)) {
    if (i < mid) {
      Henchman* enchman = _createHenchman_1(x, henchmanWidth, henchmanHeight);
      _map->add(enchman);
    } else if (i > mid) {
      Henchman* enchman = _createHenchman_2(x, henchmanWidth, henchmanHeight);
      _map->add(enchman);
    }

    x += gap;
    ++i;
  }
}

Boss_1::Boss_1(unsigned ID, const PhysicsBox& physicsBox, Map* map, double bonusProbability, double difficulty) noexcept
    : Boss(ID, physicsBox, map, BOSS_HP, BOSS_FIRE_DELAY, bonusProbability, difficulty) {
  spawnHenchmen(ASSET_HENCHMAN_1_LEFT_WIDTH, ASSET_HENCHMAN_1_LEFT_HEIGHT, 4);
}

Henchman* Boss_1::_createHenchman_1(int xPos, int henchmanWidth, int henchmanHeight) noexcept {
  PhysicsBox physicsBox(xPos, 0, henchmanWidth, henchmanHeight, 0, 0);
  return new Henchman_1(ASSET_HENCHMAN_1_LEFT_ID, physicsBox, _map, this, _bonusProbability, _difficulty);
}

Henchman* Boss_1::_createHenchman_2(int xPos, int henchmanWidth, int henchmanHeight) noexcept {
  PhysicsBox physicsBox(xPos, 0, henchmanWidth, henchmanHeight, 0, 0);
  return new Henchman_1(ASSET_HENCHMAN_1_RIGHT_ID, physicsBox, _map, this, _bonusProbability, _difficulty);
}

Boss_2::Boss_2(unsigned ID, const PhysicsBox& physicsBox, Map* map, double bonusProbability, double difficulty) noexcept
    : Boss(ID, physicsBox, map, BOSS_HP, BOSS_FIRE_DELAY, bonusProbability, difficulty) {
  spawnHenchmen(ASSET_HENCHMAN_2_LEFT_WIDTH, ASSET_HENCHMAN_2_LEFT_HEIGHT, 4);
}

Henchman* Boss_2::_createHenchman_1(int xPos, int henchmanWidth, int henchmanHeight) noexcept {
  PhysicsBox physicsBox(xPos, 0, henchmanWidth, henchmanHeight, 0, 0);
  return new Henchman_2(ASSET_HENCHMAN_2_LEFT_ID, physicsBox, _map, this, _bonusProbability, _difficulty);
}

Henchman* Boss_2::_createHenchman_2(int xPos, int henchmanWidth, int henchmanHeight) noexcept {
  PhysicsBox physicsBox(xPos, 0, henchmanWidth, henchmanHeight, 0, 0);
  return new Henchman_1(ASSET_HENCHMAN_2_RIGHT_ID, physicsBox, _map, this, _bonusProbability, _difficulty);
}

void Boss_2::spawnHenchmen(int henchmanWidth, int henchmanHeight, unsigned nHenchman) noexcept {
  unsigned mid = MAP_WIDTH / 2;
  unsigned lo_mid = mid - (MAP_WIDTH / 10);
  unsigned hi_mid = mid + (MAP_WIDTH / 10);
  unsigned gap = (MAP_WIDTH / (nHenchman + 1)) - 4;
  for (unsigned h = 0; h < nHenchman + 1; ++h) {
    unsigned xPos = gap * (h + 1);
    if (xPos < lo_mid) {
      Henchman* henchman = _createHenchman_1(int(xPos), henchmanWidth, henchmanHeight);
      _map->add(henchman);
    } else if (xPos > hi_mid) {
      Henchman* henchman = _createHenchman_2(int(xPos), henchmanWidth, henchmanHeight);
      _map->add(henchman);
    }
  }
}

Boss_3::Boss_3(unsigned ID, const PhysicsBox& physicsBox, Map* map, double bonusProbability, double difficulty) noexcept
    : Boss(ID, physicsBox, map, BOSS_HP, BOSS_FIRE_DELAY, bonusProbability, difficulty) {
  spawnHenchmen(ASSET_HENCHMAN_3_LEFT_WIDTH, ASSET_HENCHMAN_3_LEFT_HEIGHT, 4);
}

Henchman* Boss_3::_createHenchman_1(int xPos, int henchmanWidth, int henchmanHeight) noexcept {
  PhysicsBox physicsBox(xPos, 0, henchmanWidth, henchmanHeight, 0, 0);
  return new Henchman_2(ASSET_HENCHMAN_3_LEFT_ID, physicsBox, _map, this, _bonusProbability, _difficulty);
}

Henchman* Boss_3::_createHenchman_2(int xPos, int henchmanWidth, int henchmanHeight) noexcept {
  PhysicsBox physicsBox(xPos, 0, henchmanWidth, henchmanHeight, 0, 0);
  return new Henchman_1(ASSET_HENCHMAN_3_RIGHT_ID, physicsBox, _map, this, _bonusProbability, _difficulty);
}

void Boss_3::spawnHenchmen(int henchmanWidth, int henchmanHeight, unsigned nHenchman) noexcept {
  unsigned mid = MAP_WIDTH / 2;
  unsigned lo_mid = mid - (MAP_WIDTH / 10);
  unsigned hi_mid = mid + (MAP_WIDTH / 10);
  unsigned gap = (MAP_WIDTH / (nHenchman + 1)) - 4;
  for (unsigned h = 0; h < nHenchman + 1; ++h) {
    unsigned xPos = gap * (h + 1);
    if (xPos < lo_mid) {
      Henchman* henchman = _createHenchman_1(int(xPos), henchmanWidth, henchmanHeight);
      _map->add(henchman);
    } else if (xPos > hi_mid) {
      Henchman* henchman = _createHenchman_2(int(xPos), henchmanWidth, henchmanHeight);
      _map->add(henchman);
    }
  }
}

/*********************************************************************
*                               Henchman                             *
**********************************************************************/

const std::vector<std::array<int, 2>> Henchman::_moves = {
    {1, 0},
    {1, 0},
    {1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {-1, 0},
    {1, 0},
    {1, 0},
    {1, 0},
};

Henchman::Henchman(unsigned ID, const PhysicsBox& physicsBox, Map* map, Boss* creator, double hp, unsigned fireDelay, double bonusProbability, double difficulty) noexcept
    : Enemy(ID, physicsBox, map, hp, fireDelay, ENEMY_MAX_FIRE_DAMAGE, bonusProbability, difficulty), _creator(creator) {}

const std::vector<std::array<int, 2>>& Henchman::getMoves() const {
  return _moves;
}

void Henchman::shoot() noexcept {
  if (_remainingDelay == 0) {
    _map->add(_createBullet(xSize() / 2, xVelocity() * 0.5));
    _remainingDelay = _fireDelay;
  } else {
    --_remainingDelay;
  }
}

Henchman_1::Henchman_1(unsigned ID, const PhysicsBox& physicsBox, Map* map, Boss* creator, double bonusProbability, double difficulty) noexcept
    : Henchman(ID, physicsBox, map, creator, HENCHMAN_HP, HENCHMAN_FIRE_DELAY, bonusProbability, difficulty) {}

Henchman_2::Henchman_2(unsigned ID, const PhysicsBox& physicsBox, Map* map, Boss* creator, double bonusProbability, double difficulty) noexcept
    : Henchman(ID, physicsBox, map, creator, HENCHMAN_HP, HENCHMAN_FIRE_DELAY, bonusProbability, difficulty) {}

Henchman_3::Henchman_3(unsigned ID, const PhysicsBox& physicsBox, Map* map, Boss* creator, double bonusProbability, double difficulty) noexcept
    : Henchman(ID, physicsBox, map, creator, HENCHMAN_HP, HENCHMAN_FIRE_DELAY, bonusProbability, difficulty) {}
