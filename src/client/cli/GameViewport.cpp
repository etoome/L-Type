#include "client/cli/GameViewport.hpp"

#include <ncurses.h>

#include <cmath>

#include "client/Locale.hpp"
#include "client/cli/assets/UI/LevelUp.hpp"
#include "client/cli/utils.hpp"

constexpr double GAME_SIZE_SCALE = 0.8;

GameViewport::GameViewport() noexcept: Viewport() {
  nodelay(_inner, true);
  hideCursor();
}

GameViewport::~GameViewport() noexcept {
  wclear(_progressWin);
  wrefresh(_progressWin);
  delwin(_progressWin);

  wclear(_titleWin);
  wrefresh(_titleWin);
  delwin(_titleWin);

  nodelay(_inner, false);
  showCursor();
}

int GameViewport::_maxInnerHeight() const noexcept {
  int innerHeight = int(LINES * GAME_SIZE_SCALE) - PADDING_Y;
  if (innerHeight > CLI_MAP_HEIGHT + 1) {
    innerHeight = CLI_MAP_HEIGHT + 1;
  }
  return innerHeight;
}

int GameViewport::_maxInnerWidth() const noexcept {
  int innerWidth = int(COLS * GAME_SIZE_SCALE) - PADDING_X;
  if (innerWidth > CLI_MAP_WIDTH + 1) {
    innerWidth = CLI_MAP_WIDTH + 1;
  }
  return innerWidth;
}

void GameViewport::_drawScores(unsigned int score[MAX_PLAYERS]) const noexcept {
  wattron(_outer, COLOR_PAIR(0));
  mvwprintw(_outer, 1, 2, score[0] != 0 ? std::to_string(score[0]).c_str() : "");
  mvwprintw(_outer, 1, _width - int(log10(score[1])) - 3, score[1] != 0 ? std::to_string(score[1]).c_str() : "");
  wattroff(_outer, COLOR_PAIR(0));
}

void GameViewport::_drawHP(double hpPlayers[MAX_PLAYERS]) const noexcept {
  wattron(_outer, COLOR_PAIR(3));
  for (int i = 0; i != ceil(hpPlayers[0]); ++i) {
    mvwprintw(_outer, int(_height / 3) + i, 2, "❤");
  }

  if (hpPlayers[1] != 0) {
    for (int i = 0; i != ceil(hpPlayers[1]); ++i) {
      mvwprintw(_outer, int(_height / 3) + i, _width - 4, "❤");
    }
  }
  wattroff(_outer, COLOR_PAIR(3));
}

void GameViewport::_drawProgression(unsigned progress) const noexcept {
  unsigned levelProgress = 100 * (progress % (FRAMES_BY_LEVEL + 1)) / FRAMES_BY_LEVEL;
  unsigned level = progress / FRAMES_BY_LEVEL + 1;

  std::string levelTitle = Locale::get("LEVEL") + " " + std::to_string(level);

  wattron(_titleWin, COLOR_PAIR(0));
  wresize(_titleWin, 1, int(levelTitle.length()) + 2);
  mvwin(_titleWin, _yOffset, (COLS - (int(levelTitle.length()) + 2)) / 2);
  mvwprintw(_titleWin, 0, 1, levelTitle.c_str());
  wrefresh(_titleWin);
  wattroff(_titleWin, COLOR_PAIR(0));

  constexpr int progressBarWidth = 50 + 2;
  char progressBar[] = "[                                                  ]";  // 0[1 - 10]11
  for (unsigned idx = 1; idx <= levelProgress / 2; ++idx) {
    progressBar[idx] = '#';
  }

  std::string levelProgressPercent = " " + std::to_string(levelProgress) + "%% ";

  wattron(_progressWin, COLOR_PAIR(0));
  wresize(_progressWin, 1, progressBarWidth);
  mvwin(_progressWin, _yOffset + _height - 1, _xOffset + (_width - progressBarWidth) / 2);
  mvwprintw(_progressWin, 0, 0, progressBar);
  mvwprintw(_progressWin, 0, (progressBarWidth - int(levelProgressPercent.length())) / 2, levelProgressPercent.c_str());
  wrefresh(_progressWin);
  wattroff(_progressWin, COLOR_PAIR(0));
}

bool GameViewport::checkSize() const noexcept {
  return _maxInnerWidth() > CLI_MAP_WIDTH && _maxInnerHeight() > CLI_MAP_HEIGHT;
}

void GameViewport::drawGame(unsigned int score[MAX_PLAYERS], double hpPlayer[MAX_PLAYERS], unsigned progress, std::vector<Entity>& entities) {
  drawWindow();

  for (const Entity& entity: entities) {
    // Dying entities are not displayed
    if (entity.state == DIE_STATE) continue;

    if (entity.variant != 0) {
      wattron(_inner, COLOR_PAIR(entity.variant));
    }

    (entity.sprite)->draw(_inner, entity.yPos, entity.xPos);

    if (entity.variant != 0) {
      wattroff(_inner, COLOR_PAIR(entity.variant));
    }
  }

  _drawScores(score);
  _drawHP(hpPlayer);
  _drawProgression(progress);

  unsigned levelProgress = 100 * (progress % (FRAMES_BY_LEVEL + 1)) / FRAMES_BY_LEVEL;
  unsigned level = progress / FRAMES_BY_LEVEL + 1;
  if (levelProgress <= 3 && level != 1) {
    drawUI(UI_LEVEL_UP);
  }

  wrefresh(_outer);
  wrefresh(_inner);

  hideCursor();
}

void GameViewport::drawWindow() noexcept {
  Viewport::drawWindow();

  if (!checkSize()) {
    std::string txt = Locale::get("Terminal too small please resize");
    mvwprintw(_inner, _innerHeight() / 2, int((_innerWidth() - int(txt.length())) / 2), txt.c_str());
    wrefresh(_inner);
    return;
  }
}

int GameViewport::getInputKey() {
  return getKey(_inner, 0, 0);
}

void GameViewport::drawUI(const UI& ui) noexcept {
  int x = (_innerWidth() - ui.size) / 2;
  int y = ((_innerHeight() - int(ui.data.size())) / 2) - 5;

  for (size_t i = 0; i != ui.data.size(); ++i) {
    mvwprintw(_inner, y + int(i), x, "%s", ui.data[i].c_str());
  }

  wrefresh(_inner);
  hideCursor();
}
