#pragma once

#include <utils.hpp>

#include "Error.hpp"
#include "client/Locale.hpp"
#include "client/cli/SelectionMenu.hpp"
#include "client/cli/Window.hpp"

template<typename Data>
int Window::drawSelectionMenu(const std::string& title, int defaultOption, const std::vector<Data>& vect, std::function<std::string(const Data&)> fct, const std::vector<std::string>& rawVect) {
  _eraseWindow();
  _currentWindow = new SelectionMenu(title, defaultOption);

  for (const Data& data: vect) {
    dynamic_cast<SelectionMenu*>(_currentWindow)->addOptions(fct(data));
  }

  for (const std::string& data: rawVect) {
    dynamic_cast<SelectionMenu*>(_currentWindow)->addOptions(data);
  }

  _currentWindow->drawWindow();

  return dynamic_cast<SelectionMenu*>(_currentWindow)->chooseOption();
}

template<typename This>
void Window::processGameInput(CallbackFunction<int, This> fct, This* objPtr) {
  if (!_currentGame) throw FatalError("There is no active game.");

  int key = _currentGame->getInput();

  if (key == GAME_KEY_ESC) {
    Viewport* tmp = _currentWindow;

    // Ask for confirmation
    _currentWindow = new SelectionMenu("Pause", 0, Locale::get("Quit Game"), Locale::get("Resume"));
    _currentWindow->drawWindow();
    if (dynamic_cast<SelectionMenu*>(_currentWindow)->chooseOption() == 0) {
      key = GAME_KEY_ESC;
    } else {
      key = INVALID_KEY;
    }

    delete _currentWindow;
    _currentWindow = tmp;
  }

  (objPtr->*fct)(key);

  if (key == GAME_KEY_ESC) {
    throw ExitSignal();
  }
}
