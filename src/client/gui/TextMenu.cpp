#include "client/gui/TextMenu.hpp"

#include "Error.hpp"
#include "client/gui/utils.hpp"
#include "constants.hpp"

const std::string SUBWINDOW = IMG_PATH + "subwindow_background.png";
constexpr int PADDING = 20;  //px

TextMenu::TextMenu(Renderer* renderer, const std::string& text, const Font& font, bool window): Menu(renderer), _window(window) {
  _texture = new Texture{renderer->createTextureFromText(text, font, _renderer->width() / 3), {}};
  SDL_QueryTexture(_texture->texture, NULL, NULL, &_texture->geometry.w, &_texture->geometry.h);
  _texture->geometry.x = (_renderer->width() / 2) - _texture->geometry.w / 2;
  _texture->geometry.y = (_renderer->height() / 2) - _texture->geometry.h / 2;
}

TextMenu::~TextMenu() {
  SDL_DestroyTexture(_texture->texture);
  delete _texture;
}

void TextMenu::_loadChanges() {
  if (_window) {
    SDL_Rect subwindowGeometry = {
        _texture->geometry.x - PADDING,
        _texture->geometry.y - PADDING,
        _texture->geometry.w + 2 * PADDING,
        _texture->geometry.h + 2 * PADDING,
    };
    _renderer->addImage(SUBWINDOW, subwindowGeometry);
  }

  _renderer->addTexture(*_texture);
}

void TextMenu::waitExit() {
  flushInputs();

  bool quit = false;
  do {
    while (!quit && SDL_WaitEventTimeout(&_event, TICK / 1000)) {
      SDL_PollEvent(&_event);
      switch (_event.type) {
        case SDL_QUIT:
          // TODO: confirmation?
          throw FatalError("Game exit");
        case SDL_KEYDOWN:
          quit = true;
      }
    }

    refreshView();
  } while (!quit);
}
