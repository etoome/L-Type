#pragma once

#include <array>
#include <deque>
#include <map>
#include <vector>

#include "client/cli/GameViewport.hpp"
#include "client/cli/assets/Sprite.hpp"
#include "constants.hpp"

class Game {
 public:
  using Mapping = std::array<std::size_t, MAX_PLAYERS>;

 private:
  using CheatCodes = std::map<int, std::array<int, CHEAT_CODE_LENGHT>>;
  static CheatCodes _cheatCodes;

  using Entity = GameViewport::Entity;
  GameViewport* _gameViewport = new GameViewport();

  unsigned int _score[MAX_PLAYERS] = {0, 0};
  double _hpPlayers[MAX_PLAYERS] = {0, 0};
  unsigned _progress = 0;
  std::vector<Entity> _entities = {};

  std::array<std::size_t, MAX_PLAYERS> _userMapping = {0, 1};
  std::deque<int> _lastInputs = {};
  long _lastInputTimestamp = 0;

 public:
  Game(const std::array<std::size_t, MAX_PLAYERS>& mapping) noexcept;
  ~Game() = default;
  Game(const Game&) = delete;
  Game& operator=(const Game&) = delete;

  int getInput() noexcept;

  void updateGameState(unsigned progress, const unsigned score[2], const double hpPlayers[2]);
  void addEntity(const Entity&);
  void clearEntities();
  void drawWindow();
};
