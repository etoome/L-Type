/****************
 * Shared file  *
 ****************/

#include "client/KeyMapping.hpp"

#include <stdexcept>

#ifdef GUI
#include <SDL2/SDL.h>
constexpr int KEY_BACKSPACE = SDLK_BACKSPACE;
constexpr int KEY_UP = SDLK_UP;
constexpr int KEY_LEFT = SDLK_LEFT;
constexpr int KEY_DOWN = SDLK_DOWN;
constexpr int KEY_RIGHT = SDLK_RIGHT;
constexpr int KEY_ENTER = SDLK_RETURN;
constexpr int KEY_ESC = SDLK_ESCAPE;
#else
#include <ncurses.h>
constexpr int KEY_ESC = KEY_BACKSPACE;
#endif

KeyMapping::Mapping KeyMapping::_keyboardMapping = {
    {
        GAME_KEY_ESC,
        {KEY_ESC, KEY_BACKSPACE},
    },
    {
        GAME_KEY_UP,
        {'z', KEY_UP},
    },
    {
        GAME_KEY_LEFT,
        {'q', KEY_LEFT},
    },
    {
        GAME_KEY_DOWN,
        {'s', KEY_DOWN},
    },
    {
        GAME_KEY_RIGHT,
        {'d', KEY_RIGHT},
    },
    {
        GAME_KEY_SHOOT,
        {' ', KEY_ENTER},
    },
};

std::map<int, std::string> KeyMapping::keyPrint = {
    {KEY_ESC, ""},
    {KEY_BACKSPACE, ""},
    {'z', "Z"},
    {'q', "Q"},
    {'s', "S"},
    {'d', "D"},
    {KEY_ENTER, "[Enter]"},
    {KEY_LEFT, "[Left]"},
    {KEY_RIGHT, "[Right]"},
    {KEY_UP, "[Up]"},
    {KEY_DOWN, "[Down]"},
    {' ', "[Space]"},
};

KeyMapping::Mapping& KeyMapping::get() noexcept {
  return _keyboardMapping;
}

int KeyMapping::normalizeInput(int key, int gameKey) noexcept {
  if (matches(key, gameKey)) {
    return gameKey;
  } else {
    return key;
  }
}

int KeyMapping::normalizeGameInput(int key, const std::array<std::size_t, 2>& userMapping) noexcept {
  int input = INVALID_KEY;

  for (const KeyMapping::Mapping::value_type& mapping: _keyboardMapping) {
    int mapKey1 = mapping.second[userMapping[0]];
    int mapKey2 = mapping.second[userMapping[1]];

    if (key == mapKey1 || key == mapKey2) {
      int p = mapKey1 != key;

      input = mapping.first;

      input <<= 1;
      input += int(p);

      break;
    }
  }

  return input;
}

bool KeyMapping::matches(int key, int gameKey) noexcept {
  try {
    for (int k: KeyMapping::get().at(gameKey)) {
      if (k == key) {
        return true;
      }
    }
    return false;
  } catch (const std::out_of_range&) {
    return false;
  }
}
