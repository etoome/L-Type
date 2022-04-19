#pragma once

#include <array>
#include <deque>
#include <map>

#include "client/gui/Activity.hpp"
#include "client/gui/KeyHandler.hpp"
#include "client/gui/Renderer.hpp"
#include "client/gui/assets/UI/UI.hpp"
#include "client/gui/utils.hpp"
#include "client/gui/Menu.hpp"
#include "constants.hpp"

class Game: public Activity, public Menu {
 private:
  KeyHandler _keyHandler = {};

  using CheatCodes = std::map<int, std::array<int, CHEAT_CODE_LENGHT>>;
  static CheatCodes _cheatCodes;

  Texture* _progressTexture = nullptr;
  Texture* _scoreTextures[MAX_PLAYERS] = {nullptr, nullptr};

  unsigned int _score[MAX_PLAYERS] = {0, 0};
  double _hpPlayers[MAX_PLAYERS] = {0, 0};
  unsigned _progress = 0;

  SDL_Event _event = {};
  std::deque<int> _lastInputs = {};
  long _lastInputTimestamp = 0;

  void _clearTextures();
  void _drawScores();
  void _drawHP() const;
  void _drawProgression();
  virtual void _loadChanges() override;

 public:
  Game(Renderer*, const Mapping& mapping);
  ~Game() override;
  Game(const Game&) = delete;
  Game& operator=(const Game&) = delete;

  void waitForInput();
  int getInput(int nPlayer);

  void updateGameState(unsigned progress, const unsigned score[2], const double hpPlayers[2]);

  void drawUI(const UI&);
};
