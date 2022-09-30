/****************
 * Shared file  *
 ****************/

#include "client/Client.hpp"

#include <unistd.h>

#include <cmath>
#include <ctime>
#include <exception>
#include <functional>
#include <vector>

#include "MessageData.hpp"
#include "client/KeyMapping.hpp"
#include "constants.hpp"
#include "utils.hpp"

const std::string LOG_DIR = "/tmp/l-type.log/";
const std::string DEFAULT_PACK_NAME = "StorWors";

Client::Client() noexcept: _errorHandler(LOG_DIR) {}

Client::~Client() noexcept {
  stop();
}

void Client::start() noexcept {
  try {
    while (!_loginScreen()) {
      if (_exitScreen()) {
        throw Error("Could not connect");
      }
    }
    _mainMenuScreen();

  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

void Client::stop() noexcept {
  // Nothing for now
}

bool Client::_exitScreen() {
  std::vector<std::string> options = {Locale::get("Exit"), Locale::get("Back")};
  return !_window.drawSelectionMenu("", 0, options);
}

bool Client::_loginScreen() {
  bool logged;
  std::vector<std::string> options = {Locale::get("Sign In"), Locale::get("Sign Up")};
  switch (_window.drawSelectionMenu("L-Type", 0, options)) {
    case -1:
      logged = false;
      break;
    case 0:
      logged = _signin();
      break;
    case 1:
      logged = _signup();
      break;
  }
  return logged;
}

bool Client::_signin() {
  std::string username;
  std::string password;
  ClientInfo ci(false);

  try {
    username = _window.drawInputMenu(Locale::get("Username:"), USERNAME_MIN, USERNAME_MAX);
    password = _window.drawInputMenu(Locale::get("Password:"), PASSWORD_MIN, 64, true);

    ci = _communicationAPI.signIn(username, password);

    while (!ci.connected) {
      username = _window.drawInputMenu(Locale::get("Password or username incorrect. Try again"), USERNAME_MIN, USERNAME_MAX);
      password = _window.drawInputMenu(Locale::get("Password:"), PASSWORD_MIN, 64, true).empty();
    }
  } catch (const ExitSignal& s) {
    return false;
  }

  return ci.connected;
}

bool Client::_signup() {
  std::string username;
  std::string password;
  std::string passwordConfirm;
  ClientInfo ci(false);

  try {
    username = _window.drawInputMenu(Locale::get("Username:"), USERNAME_MIN, USERNAME_MAX);

    do {
      password = _window.drawInputMenu(Locale::get("Password:"), PASSWORD_MIN, 64, true);
      passwordConfirm = _window.drawInputMenu(Locale::get("Confirm Password:"), PASSWORD_MIN, 64, true);

      while (password != passwordConfirm) {
        password = _window.drawInputMenu(Locale::get("Passwords didn't match. Try again:"), PASSWORD_MIN, 64, true);
        passwordConfirm = _window.drawInputMenu(Locale::get("Confirm Password:"), PASSWORD_MIN, 64, true);
      }

      ci = _communicationAPI.signUp(username, password);

      if (!ci.connected) {
        username = _window.drawInputMenu(Locale::get("Username already taken. Try again:"), USERNAME_MIN, USERNAME_MAX);
      }
    } while (!ci.connected);
  } catch (const ExitSignal& s) {
    return false;
  }

  return ci.connected;
}

void Client::_mainMenuScreen() {
  bool quit = false;
  do {
    std::vector<std::string> options = {
        Locale::get("Play Game"),
        Locale::get("Level Editor"),
        Locale::get("Packs"),
        Locale::get("Profile"),
        Locale::get("Follows"),
        Locale::get("Leaderboard"),
        Locale::get("Settings"),
        Locale::get("Credits"),
    };

    if (_communicationAPI.isAdmin()) {
      options.push_back(Locale::get("Admin"));
    }

    switch (_window.drawSelectionMenu("L-Type", 0, options)) {
      case -1:
        quit = _exitScreen();
        break;
      case 0:
        _gameScreen();
        break;
      case 1:
        _sandboxScreen();
        break;
      case 2:
        _packsScreen();
        break;
      case 3:
        _profileScreen(_communicationAPI.getUsername());
        break;
      case 4:
        _followsScreen();
        break;
      case 5:
        _leaderboardScreen();
        break;
      case 6:
        _settingsScreen();
        break;
      case 7:
        _creditsScreen();
        break;
      case 8:
        _adminScreen();
        break;
    }
  } while (!quit);
}

/**********************************************************************
 *                               GAME                                 *
 **********************************************************************/

bool Client::_levelInfoScreen(const LevelInfo& lvlInfo) {
  bool quit = false;
  do {
    time_t t(lvlInfo.createdTimestamp);
    tm* time = localtime(&t);
    char timeStr[255];
    strftime(timeStr, 255, "%Y-%m-%d", time);

    std::vector<std::string> options = {
        {std::string(lvlInfo.name)},
        {Locale::get("Created by") + " " + std::string(lvlInfo.creator)},
        {Locale::get("Created on") + " " + std::string(timeStr)},
        {Locale::get("Rating of ") + std::to_string(lvlInfo.rate) + Locale::ratingSentence(lvlInfo.rate)},
        {Locale::get("Start game")},
    };

    switch (_window.drawSelectionMenu(Locale::get("Level info"), 0, options)) {
      case -1:
        return false;
        break;
      case 3: {
        int c = _window.drawSelectionMenu(Locale::get("Rate level"), 0, std::vector<std::string>({"0", "1", "2", "3", "4", "5"}));
        if (c != -1) {
          _communicationAPI.rateLevel(lvlInfo.id, unsigned(c));
          return false;
        }
      } break;
      case 4:
        quit = true;
        break;
    }
  } while (!quit);

  return true;
}

bool Client::_chooseLevelScreen(GameSettings& settings) {
  bool quit = false;
  do {
    std::vector<LevelInfo> levels;
    _communicationAPI.getLevels(levels, "", 10, 0);

    std::function<std::string(const LevelInfo&)> printLevel = [](const LevelInfo& lvl) {
      return std::string(lvl.name) + " (by " + std::string(lvl.creator) + ")";
    };

    int l = _window.drawSelectionMenu(Locale::get("Choose Level"), 0, levels, printLevel);
    if (l == -1) return false;

    if (_levelInfoScreen(levels[l])) {
      settings.levelID = levels[l].id;
      quit = true;
    }
  } while (!quit);

  return true;
}

void Client::_controlsScreen(bool secondPlayer) {
  if (!secondPlayer) {
    _setPlayerControls(bool(0));
  } else {
    std::vector<std::string> options = {Locale::get("Player 1"), Locale::get("Player 2")};
    int u = _window.drawSelectionMenu(Locale::get("Controls"), 0, options);
    if (u == -1) return;
    _setPlayerControls(bool(u));
  }
}

void Client::_setPlayerControls(bool playerId) {
  KeyMapping::Mapping keyboardMapping = KeyMapping::get();
  std::vector<std::size_t> options = {{0, 1}};

  std::function<std::string(const std::size_t&)> callback = [&keyboardMapping](const std::size_t& mapIndex) {
    std::string option;
    for (const KeyMapping::Mapping::value_type& item: keyboardMapping) {
      option += KeyMapping::keyPrint.at(item.second.at(mapIndex)) + " ";
    };
    return option + "\b";
  };

  int c;
  switch (c = _window.drawSelectionMenu(Locale::get("Controls"), int(_userMapping[playerId]), options, callback)) {
    case -1:
      break;
    default:
      if (_userMapping[!playerId] == unsigned(c)) {
        _userMapping[!playerId] = _userMapping[playerId];
      }
      _userMapping[playerId] = unsigned(c);
  }
}

bool Client::_setGameScreen() {
  GameSettings settings;

  bool validGame = false;
  while (!validGame) {
    // Set texts to display
    std::string gameMode = (settings.secondPlayer) ? Locale::get("Multi") : Locale::get("Solo");

    std::string difficulty;
    if (settings.difficulty <= 0.33) {
      difficulty = Locale::get("Easy");
    } else if (settings.difficulty <= 0.66) {
      difficulty = Locale::get("Medium");
    } else {
      difficulty = Locale::get("Hard");
    }

    std::string bonusProbability;
    if (settings.bonusProbability <= 0.1) {
      bonusProbability = Locale::get("Low");
    } else if (settings.bonusProbability <= 0.2) {
      bonusProbability = Locale::get("Medium");
    } else {
      bonusProbability = Locale::get("High");
    }

    std::string friendlyFire = (settings.friendlyFire) ? Locale::get("Enabled") : Locale::get("Disabled");

    // Set options with these texts
    std::vector<std::string> options;
#ifdef GUI
    options.push_back(Locale::get("Start Campaign"));
    options.push_back(Locale::get("Choose a level"));
#else
    options.push_back(Locale::get("Start Game"));
#endif
    options.push_back(Locale::get("Controls"));
    options.push_back(Locale::get("Game Mode:") + " " + gameMode);
    options.push_back(Locale::get("Initial Lives:") + " " + std::to_string(settings.initialLives));
    options.push_back(Locale::get("Difficulty:") + " " + difficulty);
    options.push_back(Locale::get("Bonus Probability:") + " " + bonusProbability);
    options.push_back(Locale::get("Friendly Fire:") + " " + friendlyFire);

    int c = _window.drawSelectionMenu(Locale::get("Game Settings"), 0, options);
    if (c == -1) return false;

    int t = 0;
    if (c == t) {
      _window.drawCountdown(3);
      validGame = _communicationAPI.createGame(settings);
    }
#ifdef GUI
    else if (c == ++t) {
      if (_chooseLevelScreen(settings)) {
        validGame = _communicationAPI.createGame(settings);
      }
    }
#endif
    // t = last optional (GUI/CLI) option
    else if (c == t + 1) {
      _controlsScreen(settings.secondPlayer);
    } else if (c == t + 2) {
      std::vector<std::string> options = {Locale::get("Solo"), Locale::get("Multi")};
      if (_window.drawSelectionMenu(Locale::get("Game Mode"), settings.secondPlayer, options) == 1) {
        settings.secondPlayer = _loginScreen();
      } else {
        _communicationAPI.removeSecondPlayer();
        settings.secondPlayer = false;
      }
    } else if (c == t + 3) {
      std::vector<std::string> options = {"1", "2", "3", "4", "5", "6", "7"};
      if ((c = _window.drawSelectionMenu(Locale::get("Initial Lives"), int(settings.initialLives) - 1, options)) != -1) {
        settings.initialLives = unsigned(c) + 1;
      }
    } else if (c == t + 4) {
      std::vector<std::string> options = {Locale::get("Easy"), Locale::get("Medium"), Locale::get("Hard")};
      if ((c = _window.drawSelectionMenu(Locale::get("Difficulty"), int(round(settings.difficulty / 0.33)) - 1, options)) != -1) {
        settings.difficulty = 0.33 * (c + 1);
      }
    } else if (c == t + 5) {
      std::vector<std::string> options = {Locale::get("Low"), Locale::get("Medium"), Locale::get("High")};
      if ((c = _window.drawSelectionMenu(Locale::get("Bonus Probability"), int(round(settings.bonusProbability / 0.1)) - 1, options)) != -1) {
        settings.bonusProbability = 0.1 * (c + 1);
      }
    } else if (c == t + 6) {
      std::vector<std::string> options = {Locale::get("Disabled"), Locale::get("Enabled")};
      if ((c = _window.drawSelectionMenu(Locale::get("Friendly Fire"), settings.friendlyFire, options)) != -1) {
        settings.friendlyFire = c;
      }
    }
  }

  return true;
}

void Client::_gameScreen() {
  if (_setGameScreen() == false) return;

  _window.initGame(_userMapping);

  int gameState = 0;
  do {
    std::vector<EntityFrame> entities;
    RefreshFrame refreshFrame = _communicationAPI.getGameState(entities);
    gameState = refreshFrame.gameState;

    long latency = getTimestamp() - refreshFrame.timestamp;  // µs

    _window.clearLabels();
    _window.drawLabel(std::to_string(latency / 1000) + "ms     ", 1, 1);

    if (latency >= MAX_LATENCY) continue;

    if (gameState == 0) {
      _window.refreshGame(refreshFrame, entities);

      try {
        _window.processGameInput<CommunicationAPI>(&CommunicationAPI::sendGameInput, &_communicationAPI);
      } catch (const ExitSignal& s) {
        gameState = -1;  // End game
      }
    }
  } while (gameState == 0);

  if (gameState == 1) {
    _window.win();
  } else {
    _window.loose();
  }

  _window.clearLabels();
  _window.finishGame();
  _communicationAPI.quitGame();
}

/**********************************************************************
 *                             SANDBOX                                *
 **********************************************************************/

bool Client::_setSandboxScreen() {
  std::vector<LevelInfo> levels;
  _communicationAPI.getLevels(levels, _communicationAPI.getUsername(), 9, 0);

  std::function<std::string(const LevelInfo&)> printLevel = [](const LevelInfo& lvl) {
    return lvl.name;
  };

  std::vector<std::string> rawOptions = {Locale::get("Create")};

  int c = _window.drawSelectionMenu(Locale::get("Choose Level"), 0, levels, printLevel, rawOptions);
  if (c == -1) return false;

  if (c == levels.size()) {
    std::string levelName;
    try {
      levelName = _window.drawInputMenu("Level name", LEVEL_MIN, LEVEL_MAX);
    } catch (const ExitSignal& s) {
      return false;
    }
    return _communicationAPI.createSandbox({-1, levelName});
  } else {
    return _communicationAPI.createSandbox({levels[c].id});
  }
}

#ifdef GUI
void Client::_sandboxScreen() {
  if (_setSandboxScreen() == false) return;

  _window.launchSandbox(&CommunicationAPI::sendSandboxEdition, &CommunicationAPI::getLvlProgress, &_communicationAPI, _userMapping);
  _communicationAPI.quitSandbox();
}
#else
void Client::_sandboxScreen() {
  _window.drawTextMenu("This menu is only available in the GUI version of the game.");
}
#endif

/**********************************************************************
 *                              SOCIAL                                *
 **********************************************************************/

void Client::_profileScreen(const std::string& username) {
  PlayerInfo playerInfo = _communicationAPI.getPlayerInfo(username);
  bool ownProfile = username == _communicationAPI.getUsername();

  bool quit = false;
  do {
    std::vector<std::string> options = {
        {Locale::get("Username:") + " " + username},
        {Locale::get("Best score:") + " " + std::to_string(playerInfo.bestScore)},
        {Locale::get("Experience:") + " " + std::to_string(playerInfo.xp)},
    };

    if (!ownProfile) {
      options.push_back(Locale::get((playerInfo.isFollowed) ? "Unfollow" : "Follow"));
    }

    switch (_window.drawSelectionMenu(Locale::get("Profile"), 0, options)) {
      case -1:
        quit = true;
        break;
      case 3:
        _manageFollowsScreen(username, !playerInfo.isFollowed);
        quit = true;
        break;
    }
  } while (!quit);
}

void Client::_followsScreen() {
  bool quit = false;
  do {
    std::vector<PlayerInfo> users;
    _communicationAPI.getFollows(users, _communicationAPI.getUsername());

    std::function<std::string(const PlayerInfo&)> printPlayer = [](const PlayerInfo& playerInfo) {
      return std::string(playerInfo.username) + (playerInfo.isFollowingMe ? " 🤝 " : "");
    };

    std::vector<std::string> options = {Locale::get("🔍 Search")};
    int c = _window.drawSelectionMenu(Locale::get("Follows"), 0, users, printPlayer, options);

    if (c == -1) {
      quit = true;
    } else if (c == int(users.size())) {
      _followScreen();
    } else {
      std::string username = users[std::size_t(c)].username;
      _profileScreen(username);
    }
  } while (!quit);
}

void Client::_followScreen() {
  std::string usernameToFollow;

  try {
    usernameToFollow = _window.drawInputMenu(Locale::get("Search Username:"), USERNAME_MIN, USERNAME_MAX);

    while (usernameToFollow == _communicationAPI.getUsername() || !_communicationAPI.follow(usernameToFollow)) {
      if (usernameToFollow == _communicationAPI.getUsername()) {
        _window.drawTextMenu("Segmentation Fault (x.x)");
        return;
      }
      usernameToFollow = _window.drawInputMenu(Locale::get("Username not found. Try again:"), USERNAME_MIN, USERNAME_MAX);
    }
  } catch (const ExitSignal& s) {
    // Nothing
  }
}

void Client::_leaderboardScreen() {
  bool quit = false;
  do {
    std::vector<PlayerInfo> users;
    _communicationAPI.getLeaderboard(users, 10, 0);

    std::function<std::string(const PlayerInfo&)> callback = [](const PlayerInfo& playerInfo) {
      return std::string(playerInfo.username) + "    " + std::to_string(playerInfo.bestScore);
    };

    int chosenOption = _window.drawSelectionMenu(Locale::get("Leaderboard"), 0, users, callback);
    switch (chosenOption) {
      case -1:
        quit = true;
        break;
      default:
        _profileScreen(users[std::size_t(chosenOption)].username);
    }
  } while (!quit);
}

void Client::_manageFollowsScreen(const std::string& username, bool add) {
  std::string promptText;

  if (add) {
    if (_communicationAPI.follow(username)) {
      promptText = Locale::get("You follow") + " " + username;
    } else {
      promptText = Locale::get("You already follow this user.");
    }
  } else {
    if (_communicationAPI.unfollow(username)) {
      promptText = Locale::get("You unfollow") + " " + username;
    } else {
      promptText = Locale::get("You don't follow this user");
    }
  }

  _window.drawTextMenu(promptText);
}

/**********************************************************************
 *                               PACKS                                *
 **********************************************************************/

#ifdef GUI
void Client::_packsScreen() {
  std::string username = _communicationAPI.getUsername();

  std::vector<Pack> packs;
  _communicationAPI.getPacks(packs, username);
  packs.insert(packs.begin(), {0, DEFAULT_PACK_NAME, true});

  int prevPack = _window.selectedPack();

  std::function<std::string(const Pack&)> printPack = [](const Pack& p) {
    return (!p.owned ? "€ " : "") + std::string(p.name) + (!p.owned ? " €" : "");
  };

  int pack = _window.drawSelectionMenu(Locale::get("Packs"), prevPack, packs, printPack);
  if (pack == -1) return;

  bool owned = false;
  for (const auto& p: packs) {
    if (p.id == pack) {
      owned = p.owned;
      break;
    }
  }

  if (!owned) {
    std::string key;
    try {
      key = _window.drawInputMenu(Locale::get("This pack is locked! Enter a key to unlock corresponding pack:"));
    } catch (const ExitSignal& s) {
      return;
    }

    if (_communicationAPI.activatePack(key, username)) {
      _window.drawTextMenu(Locale::get("Pack activated"));
      _window.setPack(pack);
    } else {
      _window.drawTextMenu(Locale::get("Key invalid"));
    }
  } else {
    _window.setPack(pack);
  }
}
#else
void Client::_packsScreen() {
  _window.drawTextMenu("This menu is only available in the GUI version of the game.");
}
#endif

/**********************************************************************
 *                             SETTINGS                               *
 **********************************************************************/

void Client::_setLanguageScreen() {
  std::vector<std::string> options = {"English", "Français"};
  switch (_window.drawSelectionMenu(Locale::get("Set Language"), int(Locale::getLanguageNumber()), options)) {
    case 0:
      Locale::setLanguage("en");
      break;
    case 1:
      Locale::setLanguage("fr");
      break;
  }
}

void Client::_settingsScreen() {
  bool quit = false;
  do {
    std::vector<std::string> options = {Locale::get("Set Language")};
    switch (_window.drawSelectionMenu(Locale::get("Settings"), 0, options)) {
      case -1:
        quit = true;
        break;
      case 0:
        _setLanguageScreen();
        break;
    }
  } while (!quit);
}

/**********************************************************************
 *                               ADMIN                                *
 **********************************************************************/

void Client::_adminScreen() {
  bool quit = false;
  do {
    std::vector<std::string> options = {"Manage Pack Keys"};
    switch (_window.drawSelectionMenu("Admin", 0, options)) {
      case -1:
        quit = true;
        break;
      case 0:
        _packKeysScreen();
        break;
    }
  } while (!quit);
}

void Client::_packKeysScreen() {
  std::vector<std::string> options = {"List", "Add", "Remove"};
  switch (_window.drawSelectionMenu("Admin - Manage Pack Keys", 0, options)) {
    case 0: {
      std::vector<PackKey> packKeys;

      std::function<std::string(const PackKey&)> callback = [](const PackKey& pk) {
        return std::string(pk.key) + " (" + std::to_string(pk.uses) + ")";
      };

      _communicationAPI.getPackKeys(packKeys);
      if (!packKeys.empty()) {
        _window.drawSelectionMenu("Admin - Pack Keys", 0, packKeys, callback);
      } else {
        _window.drawTextMenu("No Key");
      }
    } break;

    case 1:
      try {
        std::string pack = _window.drawInputMenu("Pack Name");
        std::string key = _window.drawInputMenu("Key (blank for random)");

        std::string uses;
        do {
          uses = _window.drawInputMenu("Max uses [1, 10]");
          if (uses.empty()) {
            uses = "1";
          }
        } while (std::stoi(uses) < 1 || std::stoi(uses) > 10);

        key = _communicationAPI.addPackKey(pack, key, std::stoi(uses));

        if (!key.empty()) {
          _window.drawTextMenu("Key added : " + key);
        } else {
          _window.drawTextMenu("Pack invalid");
        }
      } catch (const ExitSignal& s) {
        break;
      }
      break;

    case 2:
      try {
        std::string key = _window.drawInputMenu("Key");
        _communicationAPI.removePackKey(key);
      } catch (const ExitSignal& s) {
        break;
      }
      break;
  }
}

/**********************************************************************
 *                               CREDITS                              *
 **********************************************************************/

void Client::_creditsScreen() {
  bool quit = false;
  do {
    std::vector<std::string> options = {
      {Locale::get("Fonts")},
      {Locale::get("Images")},
      {Locale::get("Audios")},
      {Locale::get("Exit")}
    };

    std::string txt;
    switch (_window.drawSelectionMenu("Credits", 0, options)) {
      case -1:
        quit = true;
        break;
      case 0:
        txt = "Space Ranger - Iconian fonts\nMetal Lord - Typodermic fonts\nVp Pixel - VP type";
        _window.drawTextMenu(txt);
        break;
      case 1:
        txt = "Game Over - Delook Creative\nLevel Up - Delook Creative\n" + Locale::get("Everything else") + " - Fredo & Loulou";
        _window.drawTextMenu(txt);
        break;
      case 2:
        txt = Locale::get("Audios") + " - Mirco Florian";
        _window.drawTextMenu(txt);
        break;
      case 3:
        quit = true;
        break;
    }
  } while (!quit);
}
