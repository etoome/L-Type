#include "client/gui/Game.hpp"

#include "Error.hpp"
#include "client/KeyMapping.hpp"
#include "client/gui/Assets.hpp"
#include "client/gui/assets/UI/LevelUp.hpp"
#include "client/gui/guiID.hpp"
#include "utils.hpp"

Game::CheatCodes Game::_cheatCodes = {
    {
        CHEAT_CODE_LIFE,
        {CHEAT_CODE_CHAR, 'c', 'h', 'a'},
    },
    {
        CHEAT_CODE_GHOST,
        {CHEAT_CODE_CHAR, 'j', 'a', 'c'},
    },
    {
        CHEAT_CODE_HULK,
        {CHEAT_CODE_CHAR, 'r', 'o', 'b'},
    },
    {
        CHEAT_CODE_SKIP_LEVEL,
        {CHEAT_CODE_CHAR, 'c', 'e', 'd'},
    },
};

Game::Game(Renderer* renderer, const Mapping& mapping): Activity(mapping), Menu(renderer) {
  for (std::size_t c = 0; c != CHEAT_CODE_LENGHT; ++c) {
    _lastInputs.push_back(INVALID_KEY);
  }

  _renderer->toggleBackgroundScrolling();

  // Flush events
  while (SDL_PollEvent(&_event)) {
  }
}

Game::~Game() {
  _clearTextures();
  _renderer->toggleBackgroundScrolling();

  // Flush events
  while (SDL_PollEvent(&_event)) {
  }
}

void Game::waitForInput() {
  int input = INVALID_KEY;

  while (input == INVALID_KEY && SDL_PollEvent(&_event)) {
    if (_event.type == SDL_QUIT) {
      throw FatalError("Game exit");
    }

    int key = INVALID_KEY;
    switch (_event.type) {
      case SDL_TEXTINPUT:
        if (_event.text.text[1] == '\0' && _event.text.text[0] == CHEAT_CODE_CHAR) {
          key = CHEAT_CODE_CHAR;
        }
        break;
      case SDL_KEYDOWN:
        if (KeyMapping::matches(_event.key.keysym.sym, GAME_KEY_ESC)) {
          throw ExitSignal();
        } else {
          key = _event.key.keysym.sym;
          input = KeyMapping::normalizeGameInput(key, _userMapping);
          if (input != INVALID_KEY) {
            _keyHandler.press(input);
          }
        }
        break;
      case SDL_KEYUP:
        key = _event.key.keysym.sym;
        input = KeyMapping::normalizeGameInput(key, _userMapping);
        if (input != INVALID_KEY) {
          _keyHandler.unpress(input);
        }
        break;
    }

    // Cheat codes
    if (key != INVALID_KEY && _event.type != SDL_KEYUP) {
      _lastInputs.pop_front();
      _lastInputs.push_back(key);
    }
  }
}

int Game::getInput(int nPlayer) {
  int input = INVALID_KEY;

  // Cheat codes
  if (_lastInputs.front() == CHEAT_CODE_CHAR) {
    for (const CheatCodes::value_type& cheatCode: _cheatCodes) {
      int ret = cheatCode.first;

      for (size_t i = 0; i != cheatCode.second.size(); ++i) {
        if (_lastInputs[i] != cheatCode.second[i]) {
          ret = 0;
          break;
        }
      }

      if (ret != 0) {
        _lastInputs.pop_front();
        _lastInputs.push_back(EMPTY_KEY);

        return cheatCode.first;
      }
    }
  }

  input = _keyHandler.getPressedKeys(nPlayer);

  // Update timestamp to avoid Client timeout
  if ((getTimestamp() - _lastInputTimestamp) / 1000000 > CLIENT_TIMEOUT - 5) {
    _lastInputTimestamp = getTimestamp();
    if (input == INVALID_KEY) {
      input = EMPTY_KEY;
    }
  }

  return input;
}

void Game::updateGameState(unsigned progress, const unsigned score[2], const double hpPlayers[2]) {
  _progress = progress;
  _score[0] = score[0];
  _score[1] = score[1];
  _hpPlayers[0] = hpPlayers[0];
  _hpPlayers[1] = hpPlayers[1];
}

void Game::_clearTextures() {
  if (_progressTexture) {
    SDL_DestroyTexture(_progressTexture->texture);
    delete _progressTexture;
  }

  for (Texture* texture: _scoreTextures) {
    if (texture) {
      SDL_DestroyTexture(texture->texture);
      delete texture;
    }
  }
}

void Game::_drawScores() {
  if (_score[0] != 0) {
    _scoreTextures[0] = new Texture{_renderer->createTextureFromText(std::to_string(_score[0]), Fonts::DEFAULT_FONT), {}};
    SDL_QueryTexture(_scoreTextures[0]->texture, NULL, NULL, &_scoreTextures[0]->geometry.w, &_scoreTextures[0]->geometry.h);
    _scoreTextures[0]->geometry.x = 0.01 * _renderer->width();
    _scoreTextures[0]->geometry.y = 0.93 * _renderer->height();
    _renderer->addTexture(*_scoreTextures[0]);
  }

  if (_score[1] != 0) {
    _scoreTextures[1] = new Texture{_renderer->createTextureFromText(std::to_string(_score[1]), Fonts::DEFAULT_FONT), {}};
    SDL_QueryTexture(_scoreTextures[1]->texture, NULL, NULL, &_scoreTextures[1]->geometry.w, &_scoreTextures[1]->geometry.h);
    _scoreTextures[1]->geometry.x = 0.99 * _renderer->width() - _scoreTextures[1]->geometry.w;
    _scoreTextures[1]->geometry.y = 0.93 * _renderer->height();
    _renderer->addTexture(*_scoreTextures[1]);
  }
}

void Game::_drawHP() const {
  for (size_t i = 0; i < ceil(_hpPlayers[0]); ++i) {
    _renderer->addImage(Assets::getFile(PACK_HUD_HEART_ID), 0.01 * _renderer->width(), 0.04 * _renderer->height() + (((0.02 * _renderer->height()) + 1) * i), 0.01 * _renderer->width() + 1, 0.01 * _renderer->width() + 1);
  }

  for (size_t i = 0; i < ceil(_hpPlayers[1]); ++i) {
    _renderer->addImage(Assets::getFile(PACK_HUD_HEART_ID), 0.99 * _renderer->width(), 0.04 * _renderer->height() + (((0.02 * _renderer->height()) + 1) * i), 0.01 * _renderer->width(), 0.01 * _renderer->width());
  }
}

void Game::_drawProgression() {
  unsigned levelProgress = 100 * (_progress % (FRAMES_BY_LEVEL + 1)) / FRAMES_BY_LEVEL;
  unsigned level = _progress / FRAMES_BY_LEVEL + 1;

  /* Bar */
  int barWidth;
  int barHeight;
  _renderer->getImageDimensions(Assets::getFile(PACK_HUD_PROGRESS_BAR_ID), barWidth, barHeight);
  int barXPos = (_renderer->width() - barWidth) / 2;
  int barYPos = _renderer->height() - barHeight * 120 / 100;
  _renderer->addImage(Assets::getFile(PACK_HUD_PROGRESS_BAR_ID), barXPos, barYPos);

  if (levelProgress != 0) {
    /* Fill */
    int fillHeight = barHeight - 2;
    int fillWidth = barWidth * levelProgress / 100;
    _renderer->addImage(Assets::getFile(PACK_HUD_PROGRESS_FILL_ID), barXPos + 1, barYPos + 1, fillWidth, fillHeight);
  }

  /* Text */
  std::string progression = "LEVEL: " + std::to_string(level);
  _progressTexture = new Texture{_renderer->createTextureFromText(progression, Fonts::MENU_FONT), {}};
  SDL_QueryTexture(_progressTexture->texture, NULL, NULL, &_progressTexture->geometry.w, &_progressTexture->geometry.h);
  _progressTexture->geometry.x = (_renderer->width() / 2) - _progressTexture->geometry.w / 2;
  _progressTexture->geometry.y = barYPos + barHeight / 4;
  _renderer->addTexture(*_progressTexture);
}

void Game::_loadChanges() {
  for (const Entity& entity: _entities) {
    int height = (entity.sprite.width * mapScaleX(_renderer)) / _renderer->getImageFormat(entity.sprite.path);
    _renderer->addImage(entity.sprite.path, {entity.xPos, entity.yPos, entity.sprite.width * mapScaleX(_renderer), height});
  }

  _clearTextures();
  _drawScores();
  _drawHP();
  _drawProgression();

  unsigned levelProgress = 100 * (_progress % (FRAMES_BY_LEVEL + 1)) / FRAMES_BY_LEVEL;
  unsigned level = _progress / FRAMES_BY_LEVEL + 1;
  if (levelProgress <= 3 && level != 1) {
    drawUI(UI_LEVEL_UP);
  }
}

void Game::drawUI(const UI& ui) {
  int uiMaxWidth = int(_renderer->width() / 2.5);

  int width, height;
  _renderer->getImageDimensions(ui.image, width, height);
  if (width > uiMaxWidth) {
    width = uiMaxWidth;
  }

  int uiHeight = width / _renderer->getImageFormat(ui.image);
  int x = (_renderer->width() - width) / 2;
  int y = (_renderer->height() - uiHeight) / 2;
  _renderer->addImage(ui.image, {x, y, width, uiHeight});
}
