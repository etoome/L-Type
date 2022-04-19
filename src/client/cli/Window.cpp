#include "client/cli/Window.hpp"

#include <ncurses.h>
#include <unistd.h>

#include <clocale>
#include <csignal>

#include "Error.hpp"
#include "assetsID.hpp"
#include "client/KeyMapping.hpp"
#include "client/cli/Assets.hpp"
#include "client/cli/Game.hpp"
#include "client/cli/GameViewport.hpp"
#include "client/cli/SelectionMenu.hpp"
#include "client/cli/utils.hpp"

Window* window = nullptr;

inline void initColors() noexcept {
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  init_pair(2, COLOR_BLUE, COLOR_BLACK);
  init_pair(3, COLOR_RED, COLOR_BLACK);
  init_pair(4, COLOR_GREEN, COLOR_BLACK);
  init_pair(5, COLOR_YELLOW, COLOR_BLACK);

  init_pair(ASSET_POWERUP_1_ID, COLOR_YELLOW, COLOR_BLACK);
  init_pair(ASSET_POWERUP_2_ID, COLOR_CYAN, COLOR_BLACK);
}

void Window::_resizeHandler(int signal) {
  if (signal != SIGWINCH) return;
  if (!window) return;

  if (window->_currentWindow) {
    endwin();
    refresh();
    clear();

    window->_currentWindow->drawWindow();
  }
}

Window::Window() {
  Assets::buildAssets();

  signal(SIGWINCH, Window::_resizeHandler);

  setlocale(LC_ALL, "");

  initscr();
  start_color();
  initColors();
  cbreak();

  if (has_colors() == false) {
    throw Error("Terminal doesn't support colors");
  }

  window = this;
}

Window::~Window() noexcept {
  window = nullptr;
  _eraseWindow();
  clearLabels();
  endwin();
  Assets::destroyAssets();
}

void Window::_eraseWindow() {
  delete _currentWindow;
}

void Window::drawLabel(const std::string& label, int yPos, int xPos) {
  WINDOW* win = newwin(0, 0, 0, 0);
  std::size_t nLabel = _labels.size();
  _labels.push_back(win);

  wresize(_labels[nLabel], 1, int(label.length()));
  mvwin(_labels[nLabel], yPos, xPos);
  mvwprintw(_labels[nLabel], 0, 0, label.c_str());
  wrefresh(_labels[nLabel]);
}

void Window::clearLabels() {
  for (WINDOW* win: _labels) {
    wclear(win);
    wrefresh(win);
    delwin(win);
  }
  _labels.clear();
}

void Window::drawCountdown(unsigned init) {
  for (unsigned i = init; i != 0; --i) {
    drawTextMenu(std::to_string(i), 900000);
  }
}

void Window::drawTextMenu(const std::string& text, unsigned displayTime) {
  _eraseWindow();
  _currentWindow = new TextViewport(text);
  _currentWindow->drawWindow();
  hideCursor();

  if (displayTime == unsigned(-1)) {
    _currentWindow->waitExit();
  } else {
    usleep(displayTime);
  }
}

std::string Window::drawInputMenu(const std::string& text, size_t minLength, size_t maxLength, bool hidden) {
  _eraseWindow();
  _currentWindow = new TextInputViewport(text);
  _currentWindow->drawWindow();
  return dynamic_cast<TextInputViewport*>(_currentWindow)->getText(minLength, maxLength, hidden);
}

void Window::_flushInputs() {
  flushinp();
}

void Window::initGame(const Game::Mapping& userMapping) {
  if (_currentGame) throw FatalError("A game is already started.");

  _currentWindow = new GameViewport();
  _currentGame = new Game(userMapping);
  GameViewport* window = dynamic_cast<GameViewport*>(_currentWindow);

  // Wait for the user to resize the window
  while (!window->checkSize()) {
    window->drawWindow();
    pause();
  }

  window->drawWindow();
}

void Window::refreshGame(const RefreshFrame& refreshFrame, const std::vector<EntityFrame>& entities) {
  if (!_currentGame) throw FatalError("There is no active game.");

  _currentGame->updateGameState(refreshFrame.progress, refreshFrame.score, refreshFrame.hpPlayers);
  _currentGame->clearEntities();
  for (const EntityFrame& entity: entities) {
    // add in _entities display vector in game
    int yPos = int(entity.yPos * mapScaleY());
    int xPos = int(entity.xPos * mapScaleX());
    _currentGame->addEntity({Assets::getSpriteById(entity.id), yPos, xPos, entity.variant, entity.state, entity.stateStep});
  }
  _currentGame->drawWindow();
}

void Window::win() {
  drawUI(UI_CONGRATULATIONS, 3, Locale::get(Locale::getRandomSentence(Locale::CONGRATULATIONS)));
}

void Window::loose() {
  drawUI(UI_GAME_OVER, 3, Locale::get(Locale::getRandomSentence(Locale::GAME_OVER)));
}

void Window::finishGame() {
  delete _currentGame;
  _currentGame = nullptr;
}

void Window::drawUI(const UI& ui, unsigned nSleepSeconds, const std::string& textAfter) {
  GameViewport* window = dynamic_cast<GameViewport*>(_currentWindow);
  if (!window) throw FatalError("There is no active game.");

  window->drawUI(ui);

  if (!textAfter.empty()) {
    sleep(nSleepSeconds);
    _flushInputs();

    Viewport* tmp = _currentWindow;
    _currentWindow = new TextViewport(textAfter);
    _currentWindow->drawWindow();

    hideCursor();
    _currentWindow->waitExit();

    delete tmp;
  }
}
