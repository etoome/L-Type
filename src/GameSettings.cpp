#include "GameSettings.hpp"

#include <cmath>

GameSettings::GameSettings(
    bool _secondPlayer,
    unsigned int _initialLives,
    double _difficulty,
    double _bonusProbability,
    bool _friendlyFire) noexcept
    : secondPlayer(_secondPlayer),
      initialLives(_initialLives),
      difficulty((_difficulty <= 1.0) ? _difficulty : 1),
      bonusProbability((_bonusProbability <= 1.0) ? _bonusProbability : 1),
      friendlyFire(_friendlyFire) {}
