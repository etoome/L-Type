#pragma once

#include <string>

inline bool ALPHANUM(int key) {
  return ((key >= '0' && key <= '9') || (key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z'));
}

inline bool VALID_CHAR(int key) {
  return (ALPHANUM(key) || (key == '_' || key == '-'));
}

inline bool VALID_STR(const std::string& str) {
  for (char c: str) {
    if (!VALID_CHAR(int(c))) return false;
  }
  return true;
}

struct ExitSignal {};

/* Get the current time in a string.
 * The time format is the same as `strftime`.
 */
std::string getStrTime(const std::string& timeFormat) noexcept;

/* Get the current timestamp.
 */
long int getTimestamp() noexcept;

/* Get the current timestamp in a string.
 */
std::string getStrTimestamp() noexcept;

/* Get a random double between min and max.
 */
double genRandomDouble(double min, double max) noexcept;

/* Get a random double between min and max.
 */
int genRandomInt(int min, int max) noexcept;
