#pragma once

#include <vector>

#include "GameSettings.hpp"
#include "MessageData.hpp"
#include "server/Activity.hpp"
#include "server/DatabaseManager.hpp"
#include "server/game/LevelManager.hpp"
#include "server/game/PhysicsEngine.hpp"

class Game: public Activity {
 private:
  PhysicsEngine _physicsEngine;
  LevelManager _levelManager;
  long _lastInteraction = 0;

  void _loadLevel();

 public:
  Game() = delete;
  ~Game() override = default;
  Game(const GameSettings&, DatabaseManager*, const std::vector<int> levelIDs) noexcept;

  bool won() const noexcept;
  bool lost() const noexcept;
  bool hasEnded() const noexcept;

  RefreshFrame getRefreshFrame() const noexcept;
  std::vector<EntityFrame>& getEntityFrames(std::vector<EntityFrame>& dest) const noexcept;

  void start();
  void refresh();

  void applyInput(int key);
};
