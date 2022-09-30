#include "client/cli/SelectionMenu.hpp"

#include "client/KeyMapping.hpp"
#include "client/cli/utils.hpp"

SelectionMenu::SelectionMenu(const std::string& title, int defaultOption): InputViewport(), _title(title) {
  _yCursor = defaultOption;
}

int SelectionMenu::_maxInnerHeight() const noexcept {
  return int(_options.size());
}

int SelectionMenu::_maxInnerWidth() const noexcept {
  std::size_t maxLength = 0;
  for (const std::string& option: _options) {
    if (option.size() > maxLength) {
      maxLength = option.size();
    }
  }
  if (_title.size() > maxLength) {
    maxLength = _title.size();
  }
  return int(maxLength);
}

void SelectionMenu::drawWindow() noexcept {
  InputViewport::drawWindow();

  std::string title = !_title.empty() ? (" " + _title + " ") : "";
  mvwprintw(_outer, 0, ((_width - int(_title.length())) / 2) - 1, title.c_str());

  int i = 0;
  for (const std::string& option: _options) {
    mvwprintw(_inner, i++, 0, option.c_str());
  }
  if (_options.size()) {
    _highlight();
  }

  wrefresh(_outer);
  wrefresh(_inner);
}

void SelectionMenu::_decCursorY() noexcept {
  // Do not move cursor if the minimum has been reached
  if (_yCursor != 0) {
    --_yCursor;
  }
}

void SelectionMenu::_incCursorY() noexcept {
  // Do not move cursor if the maximum has been reached
  if (_yCursor != int(_options.size()) - 1) {
    ++_yCursor;
  }
}

void SelectionMenu::_highlight() noexcept {
  mvwprintw(_inner, _highlighted, 0, _options[std::size_t(_highlighted)].c_str());

  _highlighted = _yCursor;

  wattron(_inner, COLOR_PAIR(1));
  mvwprintw(_inner, _highlighted, 0, _options[std::size_t(_highlighted)].c_str());
  wattroff(_inner, COLOR_PAIR(1));
}

int SelectionMenu::chooseOption() {
  hideCursor();

  // Get characters one by one
  int c;
  while ((c = KeyMapping::normalizeInput(getKey(_inner, 0, 0), GAME_KEY_SHOOT)) != GAME_KEY_SHOOT) {
    if (KeyMapping::matches(c, GAME_KEY_UP)) {
      _decCursorY();
      _highlight();
    } else if (KeyMapping::matches(c, GAME_KEY_DOWN)) {
      _incCursorY();
      _highlight();
    } else if (KeyMapping::matches(c, GAME_KEY_ESC)) {
      return -1;
    }
  }

  showCursor();
  return _yCursor;
}

SelectionMenuPreview::SelectionMenuPreview(const std::string& title, const Sprites& sprites, int defaultOption): SelectionMenu(title) {
  for (const Sprites::value_type& sprite: sprites) {
    addOptions(sprite.first);
    _sprites.push_back(sprite.second);
  }
  _yCursor = defaultOption;
}

int SelectionMenuPreview::_maxInnerWidth() const noexcept {
  std::size_t maxLength = 0;
  for (const std::string& option: _options) {
    if (option.size() > maxLength) {
      maxLength = option.size();
    }
  }
  if (_title.size() > maxLength) {
    maxLength = _title.size();
  }
  return int(maxLength) + 8;
}

void SelectionMenuPreview::_incCursorY() noexcept {
  SelectionMenu::_incCursorY();
  drawWindow();
}

void SelectionMenuPreview::_decCursorY() noexcept {
  SelectionMenu::_decCursorY();
  drawWindow();
}

void SelectionMenuPreview::drawWindow() noexcept {
  SelectionMenu::drawWindow();

  _sprites.at(unsigned(_highlighted))->draw(_inner, (int(_sprites.size()) - 2) / 2, _maxInnerWidth() - 2);
  wrefresh(_inner);
}
