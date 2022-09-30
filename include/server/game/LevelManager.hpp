#pragma once

#include <map>
#include <vector>

#include "EntityInfo.hpp"
#include "server/DatabaseManager.hpp"
#include "server/game/PhysicsEngine.hpp"


using Level = std::map<unsigned, std::vector<EntityInfo>>;

class LevelManager {
 private:
  unsigned _progress = 0;
  PhysicsEngine* _physicsEngine;
  DatabaseManager* _dbManager;

  std::vector<LevelInfo> _levels = {};
  Level* _currentLevel = nullptr;

  bool _lockedProgress = false;

  void _lockProgress() noexcept;
  void _unlockProgress() noexcept;

 public:
  LevelManager(PhysicsEngine&, DatabaseManager*, const std::vector<int> levelIDs) noexcept;
  ~LevelManager();
  LevelManager(const LevelManager&) = delete;
  LevelManager& operator=(const LevelManager&) = delete;

  bool isEnded() const;

  void nextLevel();
  void loadLevel();

  inline unsigned currentLevel() const noexcept { return _progress / FRAMES_BY_LEVEL; }
  inline unsigned levelProgress() const noexcept { return _progress % FRAMES_BY_LEVEL; }

  void skipLevel() noexcept;
};
