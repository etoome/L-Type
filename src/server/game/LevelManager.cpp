#include "server/game/LevelManager.hpp"

#include "constants.hpp"

LevelManager::LevelManager(PhysicsEngine& physEngine, DatabaseManager* dbManager, const std::vector<int> levelIDs) noexcept
    : _physicsEngine(&physEngine), _dbManager(dbManager) {
  if (levelIDs.size() == 0 || levelIDs[0] == -1) {
    _dbManager->getLevels(_levels, -1, 0, "tijl");
  } else {
    for (int lvlID: levelIDs) {
      _levels.push_back(_dbManager->getLevelInfo(lvlID));
    }
  }

  // Load first level
  _currentLevel = new Level();
  _dbManager->populateLevel(*_currentLevel, _levels[0].id);
}

LevelManager::~LevelManager() {
  delete _currentLevel;
}

void LevelManager::_lockProgress() noexcept {
  _lockedProgress = true;
}

void LevelManager::_unlockProgress() noexcept {
  _lockedProgress = false;
}

bool LevelManager::isEnded() const {
  return currentLevel() >= unsigned(_levels.size());
}

void LevelManager::nextLevel() {
  _physicsEngine->clearMap();
  _physicsEngine->resetStates();

  delete _currentLevel;
  _currentLevel = new Level();
  _dbManager->populateLevel(*_currentLevel, _levels[currentLevel()].id);
}

void LevelManager::loadLevel() {
  unsigned progress = levelProgress();
  if (progress % FPS == 0) {
    if (_currentLevel->count(progress / FPS)) {
      for (const EntityInfo& entity: _currentLevel->at(progress / FPS)) {
        _physicsEngine->newEntity(entity);
      }
    }
  }

  if (!_lockedProgress) {
    if (levelProgress() >= FRAMES_BY_LEVEL * 0.9) {
      _lockProgress();
    }

    _progress += PROGRESS_STEP;
  }

  if (_physicsEngine->getEnemyNumber() == 0) {
    _unlockProgress();
  }
}

void LevelManager::skipLevel() noexcept {
  if (currentLevel() < int(_levels.size())) {
    _progress = (currentLevel() + 1) * FRAMES_BY_LEVEL;
  }
}
