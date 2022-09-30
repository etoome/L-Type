#pragma once

/* Containes the games settings.
 * An instance of this type is used to create a `Game`.
 */
struct GameSettings {
  bool secondPlayer = false;
  unsigned int initialLives = 5;
  double difficulty = 0.5;
  double bonusProbability = 0.1;
  bool friendlyFire = false;
  int levelID = -1;

  int skins[2] = {0, 1};

  GameSettings() noexcept = default;
  GameSettings(bool _secondPlayer, unsigned int _initialLives, double _difficulty, double _bonusProbability, bool _friendlyFire) noexcept;
};
