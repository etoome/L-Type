#include "client/cli/Viewport.hpp"

#include <cmath>

#include "client/KeyMapping.hpp"
#include "client/Locale.hpp"
#include "client/cli/assets/UI/UI.hpp"
#include "client/cli/utils.hpp"
#include "utils.hpp"

constexpr double DEFAULT_SIZE_SCALE = 0.5;
constexpr double INPUT_SIZE_SCALE = 0.5;

Viewport::~Viewport() noexcept {
  wclear(_inner);
  wrefresh(_inner);
  delwin(_inner);

  wclear(_outer);
  wrefresh(_outer);
  delwin(_outer);
}

int Viewport::_maxInnerHeight() const noexcept {
  return int(LINES * DEFAULT_SIZE_SCALE) - PADDING_Y;
}

int Viewport::_maxInnerWidth() const noexcept {
  return int(COLS * DEFAULT_SIZE_SCALE) - PADDING_X;
}

int Viewport::_innerHeight() const noexcept {
  return _height - 2 * PADDING_Y;
}

int Viewport::_innerWidth() const noexcept {
  return _width - 2 * PADDING_X;
}

int Viewport::_innerOffsetY() const noexcept {
  return _yOffset + PADDING_Y;
}

int Viewport::_innerOffsetX() const noexcept {
  return _xOffset + PADDING_X;
}

void Viewport::drawWindow() noexcept {
  _height = _maxInnerHeight() + 2 * PADDING_Y;
  _width = _maxInnerWidth() + 2 * PADDING_X;

  _yOffset = (LINES - _height) / 2;
  _xOffset = (COLS - _width) / 2;

  wclear(_outer);
  wresize(_outer, _height, _width);
  mvwin(_outer, _yOffset, _xOffset);
  box(_outer, 0, 0);
  wrefresh(_outer);

  wclear(_inner);
  wresize(_inner, _innerHeight(), _innerWidth());
  mvwin(_inner, _innerOffsetY(), _innerOffsetX());
  wrefresh(_inner);
}

void Viewport::waitExit() noexcept {
  int key;
  do {
    key = getKey(_inner, 0, 0);
  } while (!KeyMapping::matches(key, GAME_KEY_ESC) && !KeyMapping::matches(key, GAME_KEY_SHOOT));
  showCursor();
}

TextViewport::TextViewport(const std::string& text) noexcept: Viewport(), _textContent(text) {}

int TextViewport::_maxInnerHeight() const noexcept {
  return int(int(_textContent.size()) / _maxInnerWidth());
}

int TextViewport::_maxInnerWidth() const noexcept {
  return (int(_textContent.size()) < Viewport::_maxInnerWidth()) ? int(_textContent.size()) : Viewport::_maxInnerWidth();
}

void TextViewport::drawWindow() noexcept {
  Viewport::drawWindow();
  mvwprintw(_inner, 0, 0, _textContent.c_str());
  wrefresh(_inner);
}

void InputViewport::_incCursorX() noexcept {
  ++_yCursor;
  // Newline if the end of the line has been reached
  if (_yCursor >= _innerWidth()) {
    ++_xCursor;
    _yCursor = 0;
  }
}

void InputViewport::_decCursorX() noexcept {
  // Move the cursor at the right place
  if (_yCursor == 0) {
    --_xCursor;
    _yCursor = _innerWidth();
  }
  --_yCursor;
}

TextInputViewport::TextInputViewport(const std::string& text) noexcept: TextViewport(text), InputViewport() {
  _xCursor = int(text.length()) / _maxInnerWidth() + MARGIN_X;
}

int TextInputViewport::_maxInnerHeight() const noexcept {
  return int(LINES * INPUT_SIZE_SCALE) - PADDING_Y;
}

int TextInputViewport::_maxInnerWidth() const noexcept {
  return int(COLS * INPUT_SIZE_SCALE) - PADDING_X;
}

std::string TextInputViewport::getText(size_t minLength, size_t maxLength, bool hidden) {
  noecho();  // Disable built-in echoing

  _yCursor = 1;
  std::string inputText;
  // Get characters one by one
  int c;
  do {
    c = getKey(_inner, _xCursor, _yCursor);

    if (c == KEY_ENTER && inputText.length() < minLength) {
      mvwprintw(_inner, _xCursor + 2, 0, ("Min " + std::to_string(minLength) + " " + Locale::get("characters")).c_str());
    } else {
      mvwprintw(_inner, _xCursor + 2, 0, "                                                    ");
    }

    // Erase a character
    if (c == KEY_BACKSPACE) {
      if (!inputText.empty()) {
        _decCursorX();
        inputText.pop_back();
        mvwprintw(_inner, _xCursor, _yCursor, " ");
        continue;
      } else {
        throw ExitSignal();
      }
    }

    // Do not accept the character if the maximum length has been reached
    // Do not accept the character if the entered character is not valid
    if (inputText.length() == maxLength || !VALID_CHAR(c)) continue;

    inputText += char(c);

    // Hide entered characters if necessary
    if (hidden) {
      mvwprintw(_inner, _xCursor, _yCursor, "*");
    } else {
      mvwprintw(_inner, _xCursor, _yCursor, "%c", c);
    }

    _incCursorX();

  } while (c != KEY_ENTER || inputText.length() < minLength);

  echo();  // Enable built-in echoing

  return inputText;
}
