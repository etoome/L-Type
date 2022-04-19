#pragma once

#include <array>

#include "ErrorHandler.hpp"
#include "GameSettings.hpp"
#include "client/CommunicationAPI.hpp"
#include "client/Locale.hpp"

#ifdef GUI
#include "client/gui/Window.hpp"
#else
#include "client/cli/Window.hpp"
#endif

/* The main client class.
 */
class Client final {
  ErrorHandler _errorHandler;
  CommunicationAPI _communicationAPI = {};
  std::array<std::size_t, MAX_PLAYERS> _userMapping = {0, 1};

  Window _window = {};

  bool _exitScreen();

  /* Prompts the user to sign in or sign up.
   */
  bool _loginScreen();
  bool _signin();
  bool _signup();

  void _mainMenuScreen();

  bool _levelInfoScreen(const LevelInfo&);
  void _controlsScreen(bool secondPlayer);
  void _setPlayerControls(bool playerId);
  bool _chooseLevelScreen(GameSettings&);
  bool _setGameScreen();
  void _gameScreen();

  bool _setSandboxScreen();
  void _sandboxScreen();

  void _profileScreen(const std::string& username);
  void _followsScreen();
  void _followScreen();
  void _leaderboardScreen();
  void _manageFollowsScreen(const std::string& username, bool add);

  void _packsScreen();

  void _setLanguageScreen();
  void _settingsScreen();

  void _adminScreen();
  void _packKeysScreen();

  void _creditsScreen();

 public:
  Client() noexcept;
  ~Client() noexcept;
  Client(const Client&) noexcept = delete;
  Client& operator=(const Client&) noexcept = delete;

  /* Start the client window.
   */
  void start() noexcept;

  /* Stop the client window.
   */
  void stop() noexcept;
};
