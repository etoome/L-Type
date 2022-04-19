#include "client/gui/SelectionMenu.hpp"

#include "Error.hpp"
#include "client/KeyMapping.hpp"
#include "client/gui/utils.hpp"
#include "constants.hpp"
#include "utils.hpp"

constexpr int MARGIN = 2;
constexpr int BUTTON_PADDING = 10;
constexpr int BUTTON_MARGIN = 5;

const std::string BUTTON = IMG_PATH + "button.png";
const std::string BUTTON_SELECTED = IMG_PATH + "button_selected.png";

SelectionMenu::SelectionMenu(Renderer* renderer, const std::string& title, int defaultOption, const std::vector<std::string>& options)
    : Menu(renderer), _selectedOption(defaultOption) {
  int rendererWidth = _renderer->width();
  int rendererHeight = _renderer->height();

  _titleTexture = new Texture{_renderer->createTextureFromText(title, Fonts::TITLE_FONT), {}};
  SDL_QueryTexture(_titleTexture->texture, NULL, NULL, &_titleTexture->geometry.w, &_titleTexture->geometry.h);
  _titleTexture->geometry.x = (rendererWidth - _titleTexture->geometry.w) / 2;
  _titleTexture->geometry.y = 0;

  for (std::size_t o = 0; o != options.size(); ++o) {
    _optionTextures.push_back({_renderer->createTextureFromText(options[o], Fonts::MENU_FONT), {}});
    _hOptionTextures.push_back({_renderer->createTextureFromText(options[o], Fonts::MENU_H_FONT), {}});

    // Calculate texture geometry
    SDL_QueryTexture(_optionTextures[o].texture, NULL, NULL, &_optionTextures[o].geometry.w, &_optionTextures[o].geometry.h);
    _optionTextures[o].geometry.x = (rendererWidth / 2 - (_optionTextures[o].geometry.w / 2));
    _optionTextures[o].geometry.y = ((_getBorderGeometry(o).h + BUTTON_MARGIN) * double(o)) + rendererHeight / 3;
    SDL_QueryTexture(_hOptionTextures[o].texture, NULL, NULL, &_hOptionTextures[o].geometry.w, &_hOptionTextures[o].geometry.h);
    _hOptionTextures[o].geometry.x = (rendererWidth / 2 - (_hOptionTextures[o].geometry.w / 2));
    _hOptionTextures[o].geometry.y = ((_getBorderGeometry(o).h + BUTTON_MARGIN) * double(o)) + rendererHeight / 3;
  }

  if (_mouseIsOver()) {
    _renderer->render();
  }
}

SelectionMenu::~SelectionMenu() {
  _renderer->setCursor(Cursors::ARROW);
  SDL_DestroyTexture(_titleTexture->texture);
  delete _titleTexture;
  for (const std::vector<Texture>& textVect: {_optionTextures, _hOptionTextures}) {
    for (const Texture& texture: textVect) {
      SDL_DestroyTexture(texture.texture);
    }
  }
}

void SelectionMenu::_loadChanges() {
  SDL_Rect& geometry = _titleTexture->geometry;

  _renderer->addTexture(*_titleTexture);

  for (std::size_t o = 0; o != nOptions(); ++o) {
    std::string backgroundImage;
    const Texture* texture = nullptr;

    if (int(o) == _selectedOption) {
      backgroundImage = BUTTON_SELECTED;
      texture = &_hOptionTextures[o];
    } else {
      backgroundImage = BUTTON;
      texture = &_optionTextures[o];
    }

    _renderer->addImage(backgroundImage, _getBorderGeometry(o));
    _renderer->addTexture(*texture);
  }
}

std::size_t SelectionMenu::nOptions() const {
  return _optionTextures.size();
}

SDL_Rect SelectionMenu::_getBorderGeometry(std::size_t nOption) const {
  return {
      _optionTextures[nOption].geometry.x - BUTTON_PADDING,
      _optionTextures[nOption].geometry.y - BUTTON_PADDING,
      _optionTextures[nOption].geometry.w + BUTTON_PADDING * 2,
      _optionTextures[nOption].geometry.h + BUTTON_PADDING * 2,
  };
}

bool SelectionMenu::_mouseIsOver() {
  for (std::size_t o = 0; o != nOptions(); ++o) {
    bool hover = false;
    if (isMouseOver(_getBorderGeometry(o))) {
      _renderer->setCursor(Cursors::HAND);
      hover = true;
      _selectedOption = int(o);
      return true;
    }
  }

  return false;
}

void SelectionMenu::_incCursor() {
  // Do not move cursor if the maximum has been reached
  if (_selectedOption != int(nOptions()) - 1) {
    ++_selectedOption;
  }
}

void SelectionMenu::_decCursor() {
  // Do not move cursor if the minimum has been reached
  if (_selectedOption != 0) {
    --_selectedOption;
  }
}

int SelectionMenu::chooseOption() {
  flushInputs();

  long int lastRefresh = 0;
  bool quit = false;
  do {
    while (!quit && (SDL_PollEvent(&_event) || SDL_WaitEventTimeout(&_event, TICK / 1000))) {
      if (getTimestamp() - lastRefresh > TICK) {
        refreshView();
        lastRefresh = getTimestamp();
      }

      switch (_event.type) {
        case SDL_QUIT:
          throw FatalError("Game exit");

        case SDL_KEYDOWN: {
          int key = getKey(_event);
          if (KeyMapping::matches(key, GAME_KEY_SHOOT)) {
            quit = true;
          } else if (KeyMapping::matches(key, GAME_KEY_UP)) {
            _decCursor();
          } else if (KeyMapping::matches(key, GAME_KEY_DOWN)) {
            _incCursor();
          } else if (KeyMapping::matches(key, GAME_KEY_ESC)) {
            return -1;
          }
        } break;

        case SDL_MOUSEMOTION:
          if (!_mouseIsOver()) {
            _renderer->setCursor(Cursors::ARROW);
          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          if (_event.button.button == SDL_BUTTON_LEFT && _mouseIsOver()) {
            quit = true;
          } else if (_event.button.button == SDL_BUTTON_RIGHT) {
            return -1;
          }
          break;
      }
    }

    if (getTimestamp() - lastRefresh > TICK) {
      refreshView();
      lastRefresh = getTimestamp();
    }
  } while (!quit);

  return _selectedOption;
}
