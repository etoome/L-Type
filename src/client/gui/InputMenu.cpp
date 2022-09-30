#include "client/gui/InputMenu.hpp"

#include <unistd.h>

#include "Error.hpp"
#include "client/gui/utils.hpp"
#include "constants.hpp"
#include "utils.hpp"

constexpr int INPUT_WIDTH = 500;
constexpr int INPUT_HEIGHT = 200;
constexpr int MARGIN = 10;

const std::string BUTTON = IMG_PATH + "button.png";

InputMenu::InputMenu(Renderer* renderer, const std::string& text) noexcept: Menu(renderer) {
  SDL_StartTextInput();

  _textTexture = new Texture{_renderer->createTextureFromText(text, Fonts::SUBTITLE_FONT, INPUT_WIDTH - 2 * MARGIN), {}};
  SDL_QueryTexture(_textTexture->texture, NULL, NULL, &_textTexture->geometry.w, &_textTexture->geometry.h);
  _textTexture->geometry.x = _renderer->width() / 2 - (_textTexture->geometry.w / 2);
  _textTexture->geometry.y = _renderer->height()/2 - (0.49 * INPUT_HEIGHT);
  _updateInputTexture("");
}

InputMenu::~InputMenu() {
  SDL_DestroyTexture(_textTexture->texture);
  delete _textTexture;
  _clearInputTexture();
}

void InputMenu::_clearInputTexture() {
  if (_inputTexture) {
    SDL_DestroyTexture(_inputTexture->texture);
    delete _inputTexture;
    _inputTexture = nullptr;
  }
}

void InputMenu::_updateInputTexture(const std::string& inputText) {
  std::string text = (!inputText.empty()) ? inputText : " ";

  _clearInputTexture();

  _inputTexture = new Texture{_renderer->createTextureFromText(text, Fonts::INPUT_FONT, INPUT_WIDTH - 2 * MARGIN), {}};
  SDL_QueryTexture(_inputTexture->texture, NULL, NULL, &_inputTexture->geometry.w, &_inputTexture->geometry.h);
  _inputTexture->geometry.x = _renderer->width() / 2 - (_inputTexture->geometry.w / 2);
  _inputTexture->geometry.y = 0.45 * _renderer->height();
}

void InputMenu::_loadChanges() {
  SDL_Rect subwindowGeometry = {
      (_renderer->width() - INPUT_WIDTH) / 2,
      (_renderer->height() - INPUT_HEIGHT) / 2,
      INPUT_WIDTH,
      INPUT_HEIGHT,
  };
  _renderer->addImage(BUTTON, subwindowGeometry);
  _renderer->addTexture(*_textTexture);
  _renderer->addTexture(*_inputTexture);
}

std::string InputMenu::enterText(std::size_t minLength, std::size_t maxLength, bool hidden) {
  flushInputs();

  std::string inputText;
  std::string displayedText;
  bool quit = false;
  do {
    while (!quit && SDL_WaitEventTimeout(&_event, TICK / 1000)) {
      SDL_PollEvent(&_event);
      switch (_event.type) {
        case SDL_QUIT:
          throw FatalError("Game exit");

        case SDL_KEYDOWN:
          if (_event.key.keysym.sym == SDLK_RETURN && inputText.length() >= minLength) {
            quit = true;
          } else if (_event.key.keysym.sym == SDLK_BACKSPACE) {
            if (!inputText.empty()) {
              inputText.pop_back();
              displayedText.pop_back();
              _updateInputTexture(displayedText);
              refreshView();
            } else {
              throw ExitSignal();
            }
          }
          break;

        case SDL_TEXTINPUT:
          if (_event.text.text[1] == '\0' && VALID_CHAR(_event.text.text[0]) && inputText.length() <= maxLength) {
            inputText += _event.text.text[0];
            displayedText += (hidden) ? '*' : _event.text.text[0];
            _updateInputTexture(displayedText);
            refreshView();
          }
          break;
      }
    }

    _updateInputTexture(displayedText);
    refreshView();
  } while (!quit);

  return inputText;
}
