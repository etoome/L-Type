#pragma once

#include <utils.hpp>  // <> to avoid confusion with client/gui/utils.hpp

#include "Error.hpp"
#include "client/Locale.hpp"
#include "client/gui/Game.hpp"
#include "client/gui/Sandbox.hpp"
#include "client/gui/SelectionMenu.hpp"
#include "client/gui/Window.hpp"

template<typename Data>
int Window::drawSelectionMenu(const std::string& title, int defaultOption, const std::vector<Data>& vect, std::function<std::string(const Data&)> fct, const std::vector<std::string>& rawVect) {
  std::vector<std::string> options;
  for (const Data& option: vect) {
    options.push_back(fct(option));
  }
  for (const std::string& option: rawVect) {
    options.push_back(option);
  }

  SelectionMenu selectionMenu(_renderer, title, defaultOption, options);
  selectionMenu.refreshView();
  return selectionMenu.chooseOption();
}

template<typename This>
void Window::processGameInput(CallbackFunction<This, int> fct, This* objPtr) {
  Game* game;
  if (!(game = dynamic_cast<Game*>(_currentActivity))) throw FatalError("There is no active game.");

  try {
    game->waitForInput();
  } catch (const ExitSignal& s) {
    // Ask for confirmation
    std::vector<std::string> options = {Locale::get("Quit Game"), Locale::get("Resume")};
    SelectionMenu selectionMenu(_renderer, "Pause", 0, options);
    selectionMenu.refreshView();

    if (selectionMenu.chooseOption() == 0) {
      (objPtr->*fct)(GAME_KEY_ESC);
      throw ExitSignal();
    }
  }

  for (int p = 0; p != MAX_PLAYERS; ++p) {
    int key = game->getInput(p);
    if (key != INVALID_KEY) {
      (objPtr->*fct)(key);
    }
  }
}

template<typename This>
void Window::launchSandbox(
    void (This::*sendSandboxEdition)(const SandboxEdition&) const,
    std::vector<EntityInfo>& (This::*sendProgress)(std::vector<EntityInfo>&, unsigned) const,
    This* objPtr,
    const Activity::Mapping& userMapping) {
  if (_currentActivity) throw FatalError("An activity is already started");
  Sandbox sandbox(_renderer, userMapping);

  {
    std::vector<EntityInfo> entities;
    (objPtr->*sendProgress)(entities, 0);
    (objPtr->*sendProgress)(entities, BOSS_PROGRESS);
    sandbox.updateEntities(entities);
  }

  bool quit = false;
  do {
    try {
      sandbox.refreshView();
      sandbox.handleEvents();
    } catch (unsigned progress) {
      std::vector<EntityInfo> entities;
      (objPtr->*sendProgress)(entities, progress);
      sandbox.updateEntities(entities);
    } catch (const SandboxEdition& edition) {
      (objPtr->*sendSandboxEdition)(edition);
    } catch (const std::array<SandboxEdition, 2>& editions) {
      (objPtr->*sendSandboxEdition)(editions[0]);
      (objPtr->*sendSandboxEdition)(editions[1]);
    } catch (ExitSignal s) {
      // Ask for confirmation
      std::vector<std::string> options = {Locale::get("Quit Sandbox"), Locale::get("Resume")};
      SelectionMenu selectionMenu(_renderer, "", 0, options);
      selectionMenu.refreshView();

      if (selectionMenu.chooseOption() == 0) {
        quit = true;
      }
    }
  } while (!quit);
}
