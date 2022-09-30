#include "client/gui/KeyHandler.hpp"

#include <algorithm>

#include "utils.hpp"

std::array<int, 5> KeyHandler::_validKeys = {
    GAME_KEY_UP,
    GAME_KEY_DOWN,
    GAME_KEY_RIGHT,
    GAME_KEY_LEFT,
    GAME_KEY_SHOOT,
};

bool KeyHandler::_isValidKey(int key) {
  for (int k: _validKeys) {
    if (k == key) {
      return true;
    }
  }
  return false;
}

void KeyHandler::press(int key) noexcept {
  bool nPlayer = key & 1;
  if (_isValidKey(key >> 1)) {
    _keyPressState[key >> 1][key & 1] = true;
  }
}

void KeyHandler::unpress(int key) noexcept {
  bool nPlayer = key & 1;
  if (_isValidKey(key >> 1)) {
    _keyPressState[key >> 1][key & 1] = false;
  }
}

int KeyHandler::getPressedKeys(bool nPlayer) noexcept {
  int sum = 0;
  for (auto& key: _keyPressState) {
    if (key.second[nPlayer]) {
      sum += key.first;
    }
  }
  _lastInputTime = getTimestamp();

  if (sum) {
    return (sum << 1) + nPlayer;
  } else {
    return INVALID_KEY;
  }
}

long int KeyHandler::lastInputTime() const {
  return _lastInputTime;
}
