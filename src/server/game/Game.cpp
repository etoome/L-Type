#include "server/game/Game.hpp"

#include <cmath>

#include "Error.hpp"
#include "assetsID.hpp"
#include "constants.hpp"
#include "server/game/Entity.hpp"
#include "server/game/players.hpp"
#include "utils.hpp"

Game::Game(const GameSettings& settings, DatabaseManager* databaseManager, const std::vector<int> levelIDs) noexcept
    : Activity(),
      _physicsEngine(settings.friendlyFire, settings.initialLives, settings.bonusProbability, settings.difficulty),
      _levelManager(_physicsEngine, databaseManager, levelIDs),
      _lastInteraction(getTimestamp()) {
  for (unsigned p = 0; p != unsigned(settings.secondPlayer) + 1; ++p) {
    unsigned nSkin = unsigned(settings.skins[p]);
    _physicsEngine.newPlayer(p, {
                                    std::get<PLAYER_INFO_ID>(playerInfo[nSkin]),
                                    {
                                        double((int(p) + 1) * MAP_WIDTH) / 3,
                                        MAP_HEIGHT / 5 * 4,
                                        std::get<PLAYER_INFO_WIDTH>(playerInfo[nSkin]),
                                        std::get<PLAYER_INFO_HEIGHT>(playerInfo[nSkin]),
                                        0,
                                        0,
                                    },
                                });
  }
}

bool Game::won() const noexcept {
  return _levelManager.isEnded();
}

bool Game::lost() const noexcept {
  if (_stopped) {
    return true;
  }

  // If Client Disconnected
  if (_lastInteraction != 0 && (getTimestamp() - _lastInteraction) / 1000000 >= CLIENT_TIMEOUT) {
    return true;
  }

  const Player* player1;
  const Player* player2;
  _physicsEngine.getPlayers(player1, player2);
  return player1->hp() == 0 && ((player2) ? player2->hp() == 0 : true);
}

bool Game::hasEnded() const noexcept {
  return won() || lost();
}

RefreshFrame Game::getRefreshFrame() const noexcept {
  const Player* player1;
  const Player* player2;
  _physicsEngine.getPlayers(player1, player2);
  return {
      (won()) ? 1 : (lost()) ? -1
                             : 0,
      getTimestamp(),
      {player1->score(), (player2) ? player2->score() : 0},
      {player1->hp(), (player2) ? player2->hp() : 0},
      _levelManager.levelProgress() + _levelManager.currentLevel() * FRAMES_BY_LEVEL,
      _physicsEngine.getEntityNumber()};
}

std::vector<EntityFrame>& Game::getEntityFrames(std::vector<EntityFrame>& dest) const noexcept {
  std::vector<Entity*> entities;
  for (Entity* entity: _physicsEngine.getAllEntities(entities)) {
    double hp = 0;
    if (PhysicalEntity* pentity = dynamic_cast<PhysicalEntity*>(entity)) {
      hp = pentity->hp();
    }

    unsigned powerUpID = 0;
    if (Player* player = dynamic_cast<Player*>(entity)) {
      powerUpID = player->powerUpID();
    } else if (Bullet* bullet = dynamic_cast<Bullet*>(entity)) {
      if (bullet->getShooter()) {
        powerUpID = (bullet->getShooter())->powerUpID();
      }
    }

    dest.push_back({
        entity->ID(),
        entity->xPos(),
        entity->yPos(),
        hp,
        entity->state(),
        entity->stateStep(),
        powerUpID,
    });
  }
  return dest;
}

void Game::start() {
  _levelManager.loadLevel();
}

void Game::refresh() {
  _physicsEngine.makeMoves();
  _physicsEngine.cleanOffScreen();
  _loadLevel();
  _physicsEngine.makeAttacks();
  _physicsEngine.checkCollisions();
  _physicsEngine.refreshStates();
}

void Game::_loadLevel() {
  unsigned currentLevel = _levelManager.currentLevel();
  unsigned levelProgression = _levelManager.levelProgress();

  if (levelProgression == 0 && currentLevel != 0) {
    _levelManager.nextLevel();
  }
  _levelManager.loadLevel();
}

void Game::applyInput(int key) {
  _lastInteraction = getTimestamp();

  switch (key) {
    case GAME_KEY_ESC:
      stop();
      throw Error("Game stopped");
      break;
    case CHEAT_CODE_LIFE:
      _physicsEngine.playersNewLife();
      break;
    case CHEAT_CODE_GHOST:
      _physicsEngine.playersToggleGhost();
      break;
    case CHEAT_CODE_HULK:
      _physicsEngine.playersToggleHulk();
      break;
    case CHEAT_CODE_SKIP_LEVEL:
      _levelManager.skipLevel();
      break;
  }

  switch (key >> 1) {
    case GAME_KEY_UP:
      _physicsEngine.setPlayerVelocityY(key & 1, -1);
      break;
    case GAME_KEY_DOWN:
      _physicsEngine.setPlayerVelocityY(key & 1, 1);
      break;
    case GAME_KEY_RIGHT:
      _physicsEngine.setPlayerVelocityX(key & 1, 1);
      break;
    case GAME_KEY_LEFT:
      _physicsEngine.setPlayerVelocityX(key & 1, -1);
      break;
    case GAME_KEY_UP_RIGHT:
      _physicsEngine.setPlayerVelocityX(key & 1, 1);
      _physicsEngine.setPlayerVelocityY(key & 1, -1);
      break;
    case GAME_KEY_UP_LEFT:
      _physicsEngine.setPlayerVelocityX(key & 1, -1);
      _physicsEngine.setPlayerVelocityY(key & 1, -1);
      break;
    case GAME_KEY_DOWN_RIGHT:
      _physicsEngine.setPlayerVelocityX(key & 1, 1);
      _physicsEngine.setPlayerVelocityY(key & 1, 1);
      break;
    case GAME_KEY_DOWN_LEFT:
      _physicsEngine.setPlayerVelocityX(key & 1, -1);
      _physicsEngine.setPlayerVelocityY(key & 1, 1);
      break;
    case GAME_KEY_SHOOT:
      _physicsEngine.playerShoot(key & 1);
      break;
    case GAME_KEY_SHOOT_UP:
      _physicsEngine.playerShoot(key & 1);
      _physicsEngine.setPlayerVelocityY(key & 1, -1);
      break;
    case GAME_KEY_SHOOT_DOWN:
      _physicsEngine.playerShoot(key & 1);
      _physicsEngine.setPlayerVelocityY(key & 1, 1);
      break;
    case GAME_KEY_SHOOT_RIGHT:
      _physicsEngine.playerShoot(key & 1);
      _physicsEngine.setPlayerVelocityX(key & 1, 1);
      break;
    case GAME_KEY_SHOOT_LEFT:
      _physicsEngine.playerShoot(key & 1);
      _physicsEngine.setPlayerVelocityX(key & 1, -1);
      break;
    case GAME_KEY_SHOOT_UP_RIGHT:
      _physicsEngine.playerShoot(key & 1);
      _physicsEngine.setPlayerVelocityX(key & 1, 1);
      _physicsEngine.setPlayerVelocityY(key & 1, -1);
      break;
    case GAME_KEY_SHOOT_UP_LEFT:
      _physicsEngine.playerShoot(key & 1);
      _physicsEngine.setPlayerVelocityX(key & 1, -1);
      _physicsEngine.setPlayerVelocityY(key & 1, -1);
      break;
    case GAME_KEY_SHOOT_DOWN_RIGHT:
      _physicsEngine.playerShoot(key & 1);
      _physicsEngine.setPlayerVelocityX(key & 1, 1);
      _physicsEngine.setPlayerVelocityY(key & 1, 1);
      break;
    case GAME_KEY_SHOOT_DOWN_LEFT:
      _physicsEngine.playerShoot(key & 1);
      _physicsEngine.setPlayerVelocityX(key & 1, -1);
      _physicsEngine.setPlayerVelocityY(key & 1, 1);
      break;
  }
}
