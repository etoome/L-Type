#pragma once

#include <vector>

#include "client/cli/Viewport.hpp"
#include "client/cli/assets/Sprite.hpp"
#include "client/cli/assets/UI/UI.hpp"
#include "constants.hpp"

class GameViewport: public Viewport {
 private:
  WINDOW* _progressWin = newwin(0, 0, 0, 0);
  WINDOW* _titleWin = newwin(0, 0, 0, 0);

  int _maxInnerHeight() const noexcept override;
  int _maxInnerWidth() const noexcept override;
  void _drawScores(unsigned int score[MAX_PLAYERS]) const noexcept;
  void _drawHP(double hpPlayers[MAX_PLAYERS]) const noexcept;
  void _drawProgression(unsigned progress) const noexcept;

 public:
  struct Entity {
    const Sprite* sprite;
    int yPos;
    int xPos;
    unsigned variant;
    unsigned state;
    unsigned stateStep;
  };

  GameViewport() noexcept;
  ~GameViewport() noexcept override;
  GameViewport(const GameViewport&) = delete;
  GameViewport& operator=(const GameViewport&) = delete;

  bool checkSize() const noexcept;
  void drawWindow() noexcept override;
  void drawGame(unsigned int score[MAX_PLAYERS], double hpPlayer[MAX_PLAYERS], unsigned progress, std::vector<Entity>& entities);
  int getInputKey();
  void drawUI(const UI&) noexcept;
};
