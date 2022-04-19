#pragma once

#include <array>
#include <map>

#include "constants.hpp"

class KeyHandler {
 private:
  static std::array<int, 5> _validKeys;

  static bool _isValidKey(int key);

  std::map<int, std::array<bool, 2>> _keyPressState = {};
  int long _lastInputTime = 0;

 public:
  KeyHandler() = default;
  ~KeyHandler() = default;

  void press(int key) noexcept;
  void unpress(int key) noexcept;

  /* Return pressed key1+key2+key3+...
   */
  int getPressedKeys(bool nPlayer) noexcept;

  long int lastInputTime() const;
};
