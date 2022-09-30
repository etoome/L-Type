#pragma once

#include <map>
#include <string>
#include <vector>

#include "constants.hpp"

class KeyMapping {
 public:
  using Mapping = std::map<int, std::vector<int>>;

 private:
  static Mapping _keyboardMapping;

 public:
  static Mapping& get() noexcept;
  static std::map<int, std::string> keyPrint;
  /* Return
   * - key
   * - gameKey if compatible key pressed
   */
  static int normalizeInput(int key, int gameKey) noexcept;
  /* Return a normalized key or INVALID_KEY.
   * The lower bit is 0 for player 1 and 1 for player 2.
   */
  static int normalizeGameInput(int key, const std::array<std::size_t, 2>& userMapping) noexcept;
  static bool matches(int key, int gameKey) noexcept;
};
