#include "client/gui/Window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <unistd.h>

#include <cstdio>
#include <string>

#include "Error.hpp"
#include "assetsID.hpp"
#include "client/gui/Assets.hpp"
#include "client/gui/Game.hpp"
#include "client/gui/InputMenu.hpp"
#include "client/gui/TextMenu.hpp"
#include "client/gui/utils.hpp"
#include "constants.hpp"

Window::Window() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    char errMessage[255];
    sprintf(errMessage, "Unable to SDL_Init: %s", SDL_GetError());
    throw FatalError(errMessage);
  }

  _window = SDL_CreateWindow("L-Type", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
  if (_window == NULL) {
    char errMessage[255];
    sprintf(errMessage, "Unable to IMG_Init: %s", SDL_GetError());
    throw FatalError(errMessage);
  }

  _renderer = new Renderer(_window);

  if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 1, 4096) == -1) {
    char errMessage[255];
    sprintf(errMessage, "Unable to open audio mix");
    throw FatalError(errMessage);
  }

  Mix_AllocateChannels(_audios.size());

  Mix_Volume(-1, VOLUME);

  Mix_Volume(PACK_AUDIO_GAME_ID, VOLUME * 2);

  _audios[PACK_AUDIO_MENU_ID] = Mix_LoadWAV(Assets::getAudio(PACK_AUDIO_MENU_ID).c_str());
  if (_audios[PACK_AUDIO_MENU_ID] == NULL) {
    char errMessage[255];
    sprintf(errMessage, "Unable to load menu music");
    throw FatalError(errMessage);
  }

  if (Mix_FadeInChannel(PACK_AUDIO_MENU_ID, _audios[PACK_AUDIO_MENU_ID], -1, AUDIO_TRANSITION) == -1) {
    char errMessage[255];
    sprintf(errMessage, "Unable to play menu music");
    throw FatalError(errMessage);
  }

  _renderer->renderSplashScreen();

  _initColors();
  Fonts::initFonts(_renderer);
  Cursors::initCursors();

  _renderer->render();
}

Window::~Window() noexcept {
  for (size_t i = 0; i < _audios.size(); i++) {
    if (_audios[i] != NULL) {
      Mix_FreeChunk(_audios[i]);
    }
  }

  Mix_CloseAudio();

  delete _currentActivity;
  delete _renderer;
  Cursors::destroyCursors();
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

void Window::_playSFX(int id) {
  if (_audios[id] != NULL) {
    Mix_FreeChunk(_audios[id]);
  }
  _audios[id] = Mix_LoadWAV(Assets::getAudio(id).c_str());
  if (_audios[id] == NULL) {
    char errMessage[255];
    sprintf(errMessage, "Unable to load music (#%d)", id);
    throw FatalError(errMessage);
  }

  if (Mix_PlayChannel(id, _audios[id], 0) == -1) {
    char errMessage[255];
    sprintf(errMessage, "Unable to play music (#%d)", id);
    throw FatalError(errMessage);
  }
}

void Window::_initColors() {
  _renderer->loadColor("black", {0, 0, 0, 255});
  _renderer->loadColor("red", {255, 0, 0, 255});
  _renderer->loadColor("white", {255, 255, 255, 255});
}

void Window::drawLabel(const std::string& label, int yPos, int xPos) {
  _renderer->addTexture({_renderer->createTextureFromText(label, Fonts::DEFAULT_FONT), xPos, yPos, 0, 0}, true);
}

void Window::clearLabels() {
  _renderer->clearPermanentTextures();
}

void Window::drawCountdown(unsigned init) {
  for (unsigned i = init; i != 0; --i) {
    TextMenu textMenu(_renderer, std::to_string(i), Fonts::TITLE_FONT, false);
    textMenu.refreshView();
    usleep(900000);
  }
}

void Window::drawTextMenu(const std::string& text, unsigned displayTime) {
  TextMenu textMenu(_renderer, text, _currentFont);
  textMenu.refreshView();
  if (displayTime == unsigned(-1)) {
    textMenu.waitExit();
  } else {
    usleep(displayTime);
  }
}

std::string Window::drawInputMenu(const std::string& text, size_t minLength, size_t maxLength, bool hidden) {
  InputMenu inputMenu(_renderer, text);
  inputMenu.refreshView();
  return inputMenu.enterText(minLength, maxLength, hidden);
}

void Window::initGame(const Activity::Mapping& userMapping) {
  if (_currentActivity) throw FatalError("An activity is already started");
  _currentActivity = new Game(_renderer, userMapping);

  Mix_FadeOutChannel(PACK_AUDIO_MENU_ID, AUDIO_TRANSITION);
}

void Window::refreshGame(const RefreshFrame& refreshFrame, const std::vector<EntityFrame>& entities) {
  Game* game;
  if (!(game = dynamic_cast<Game*>(_currentActivity))) throw FatalError("There is no active game.");

  game->updateGameState(refreshFrame.progress, refreshFrame.score, refreshFrame.hpPlayers);
  game->clearEntities();

  unsigned levelProgress = 100 * (refreshFrame.progress % (FRAMES_BY_LEVEL + 1)) / FRAMES_BY_LEVEL;
  unsigned level = refreshFrame.progress / FRAMES_BY_LEVEL + 1;

  if (levelProgress == 0) {
    if (level != 1) {
      Mix_FadeOutChannel(PACK_AUDIO_BOSS_ID, AUDIO_TRANSITION);
      _playSFX(PACK_AUDIO_LEVEL_UP_ID);
    }

    if (_audios[PACK_AUDIO_GAME_ID] != NULL) {
      Mix_FreeChunk(_audios[PACK_AUDIO_GAME_ID]);
    }
    _audios[PACK_AUDIO_GAME_ID] = Mix_LoadWAV(Assets::getAudio(PACK_AUDIO_GAME_ID).c_str());
    if (_audios[PACK_AUDIO_GAME_ID] == NULL) {
      char errMessage[255];
      sprintf(errMessage, "Unable to load game music");
      throw FatalError(errMessage);
    }
    if (Mix_FadeInChannel(PACK_AUDIO_GAME_ID, _audios[PACK_AUDIO_GAME_ID], -1, AUDIO_TRANSITION) == -1) {
      char errMessage[255];
      sprintf(errMessage, "Unable to play game music");
      throw FatalError(errMessage);
    }
  }

  for (const EntityFrame& entity: entities) {
    // Music

    switch (entity.id >> ASSET_LEN_SID) {
      case ASSET_BOSS_TYPE: {
        if (!Mix_Playing(PACK_AUDIO_BOSS_ID)) {
          Mix_FadeOutChannel(PACK_AUDIO_GAME_ID, AUDIO_TRANSITION);

          if (_audios[PACK_AUDIO_BOSS_ID] != NULL) {
            Mix_FreeChunk(_audios[PACK_AUDIO_BOSS_ID]);
          }
          _audios[PACK_AUDIO_BOSS_ID] = Mix_LoadWAV(Assets::getAudio(PACK_AUDIO_BOSS_ID).c_str());
          if (_audios[PACK_AUDIO_BOSS_ID] == NULL) {
            char errMessage[255];
            sprintf(errMessage, "Unable to load boss music");
            throw FatalError(errMessage);
          }
          if (Mix_FadeInChannel(PACK_AUDIO_BOSS_ID, _audios[PACK_AUDIO_BOSS_ID], -1, AUDIO_TRANSITION) == -1) {
            char errMessage[255];
            sprintf(errMessage, "Unable to play boss music");
            throw FatalError(errMessage);
          }
        }
      } break;
      case ASSET_PLAYER_TYPE: {
        switch (entity.state) {
          case SHOOT_STATE: {
            if (entity.stateStep == 1) {
              _playSFX(PACK_AUDIO_SHOOT_ID);
            }
          } break;
          case HURT_STATE: {
            if (entity.stateStep == 1) {
              _playSFX(PACK_AUDIO_HURT_ID);
            }
          } break;
          case RESPAWN_STATE: {
            if (entity.stateStep == 1 && refreshFrame.progress % FRAMES_BY_LEVEL) {
              _playSFX(PACK_AUDIO_RESPAWN_ID);
            }
          } break;
          case PICK_POWER_UP_STATE: {
            if (entity.stateStep == 1) {
              if (entity.variant == ASSET_POWERUP_1_ID) {
                _playSFX(PACK_AUDIO_POWER_UP_DAMAGE_ID);
              } else if (entity.variant == ASSET_POWERUP_2_ID) {
                _playSFX(PACK_AUDIO_POWER_UP_FIRE_RATE_ID);
              }
            }
          } break;
        }
      } break;
    }

    // add in _entities display vector in game
    int yPos = int(entity.yPos * mapScaleY(_renderer));
    int xPos = int(entity.xPos * mapScaleX(_renderer));
    game->addEntity({Assets::getSprite(entity.id, entity.state, entity.stateStep / 5, entity.variant), xPos, yPos});
  }

  game->refreshView();
}

void Window::win() {
  Mix_FadeOutChannel(PACK_AUDIO_BOSS_ID, AUDIO_TRANSITION);

  if (_audios[PACK_AUDIO_WIN_ID] != NULL) {
    Mix_FreeChunk(_audios[PACK_AUDIO_WIN_ID]);
  }
  _audios[PACK_AUDIO_WIN_ID] = Mix_LoadWAV(Assets::getAudio(PACK_AUDIO_WIN_ID).c_str());
  if (_audios[PACK_AUDIO_WIN_ID] == NULL) {
    char errMessage[255];
    sprintf(errMessage, "Unable to load win music");
    throw FatalError(errMessage);
  }
  if (Mix_FadeInChannel(PACK_AUDIO_WIN_ID, _audios[PACK_AUDIO_WIN_ID], 0, AUDIO_TRANSITION) == -1) {
    char errMessage[255];
    sprintf(errMessage, "Unable to play win music");
    throw FatalError(errMessage);
  }

  drawUI(UI_CONGRATULATIONS, 3, Locale::get(Locale::getRandomSentence(Locale::CONGRATULATIONS)));
}

void Window::loose() {
  Mix_FadeOutChannel(PACK_AUDIO_GAME_ID, AUDIO_TRANSITION);
  Mix_FadeOutChannel(PACK_AUDIO_BOSS_ID, AUDIO_TRANSITION);

  if (_audios[PACK_AUDIO_LOOSE_ID] != NULL) {
    Mix_FreeChunk(_audios[PACK_AUDIO_LOOSE_ID]);
  }
  _audios[PACK_AUDIO_LOOSE_ID] = Mix_LoadWAV(Assets::getAudio(PACK_AUDIO_LOOSE_ID).c_str());
  if (_audios[PACK_AUDIO_LOOSE_ID] == NULL) {
    char errMessage[255];
    sprintf(errMessage, "Unable to load loose music");
    throw FatalError(errMessage);
  }
  if (Mix_FadeInChannel(PACK_AUDIO_LOOSE_ID, _audios[PACK_AUDIO_LOOSE_ID], 0, AUDIO_TRANSITION) == -1) {
    char errMessage[255];
    sprintf(errMessage, "Unable to play loose music");
    throw FatalError(errMessage);
  }

  drawUI(UI_GAME_OVER, 3, Locale::get(Locale::getRandomSentence(Locale::GAME_OVER)));
}

void Window::finishGame() {
  delete _currentActivity;
  _currentActivity = nullptr;

  for (size_t a = 0; a != _audios.size(); ++a) {
    if (a != PACK_AUDIO_MENU_ID) {
      Mix_FadeOutChannel(a, AUDIO_TRANSITION);
    }
  }

  if (Mix_FadeInChannel(PACK_AUDIO_MENU_ID, _audios[PACK_AUDIO_MENU_ID], -1, AUDIO_TRANSITION) == -1) {
    char errMessage[255];
    sprintf(errMessage, "Unable to play menu music");
    throw FatalError(errMessage);
  }
}

void Window::drawUI(const UI& ui, unsigned nSleepSeconds, const std::string& textAfter) {
  _renderer->clearPermanentTextures();

  int uiWidth = int(_renderer->width() / 2.5);
  int uiHeight = uiWidth / _renderer->getImageFormat(ui.image);
  int x = (_renderer->width() - uiWidth) / 2;
  int y = (_renderer->height() - uiHeight) / 2;
  _renderer->addImage(ui.image, {x, y, uiWidth, uiHeight}, true);

  long int t = getTimestamp();
  while (getTimestamp() - t < nSleepSeconds * 1000000) {
    _renderer->clear();
    _renderer->render();
    usleep(TICK);
  }

  drawTextMenu(textAfter);

  _renderer->clearPermanentTextures();
}

int Window::selectedPack() {
  return Assets::getCurrentPack();
}

void Window::setPack(int pack) {
  Assets::setPack(pack);

  Mix_FadeOutChannel(PACK_AUDIO_MENU_ID, AUDIO_TRANSITION);

  if (_audios[PACK_AUDIO_MENU_ID] != NULL) {
    Mix_FreeChunk(_audios[PACK_AUDIO_MENU_ID]);
  }
  _audios[PACK_AUDIO_MENU_ID] = Mix_LoadWAV(Assets::getAudio(PACK_AUDIO_MENU_ID).c_str());
  if (_audios[PACK_AUDIO_MENU_ID] == NULL) {
    char errMessage[255];
    sprintf(errMessage, "Unable to load menu music");
    throw FatalError(errMessage);
  }

  if (Mix_FadeInChannel(PACK_AUDIO_MENU_ID, _audios[PACK_AUDIO_MENU_ID], -1, AUDIO_TRANSITION) == -1) {
    char errMessage[255];
    sprintf(errMessage, "Unable to play menu music");
    throw FatalError(errMessage);
  }
}
