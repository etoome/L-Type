#include "server/Server.hpp"

#include <unistd.h>

#include <chrono>
#include <utility>
#include <vector>

#include "Error.hpp"
#include "MessageData.hpp"
#include "SandboxSettings.hpp"
#include "constants.hpp"
#include "server/sandbox/Sandbox.hpp"
#include "server/utils.hpp"
#include "utils.hpp"

const std::string LOG_DIR = "/tmp/l-type.log/";
const std::string DB_PATH = "static/ltype.db";

bool Server::_isTokenValid(const Token& token) noexcept {
  std::string username = token.getUsername();
  std::string gameID = token.getActivityID();
  std::string secondUsername = token.getGuestUsername();
  std::string timestamp = token.getTimestamp();
  std::string sig = token.getSignature();
  return genSignature(username + gameID + secondUsername + timestamp) == sig;
}

Server::Server() noexcept: _errorHandler(LOG_DIR), _databaseManager(DB_PATH) {
  try {
    _messageExchanger.init();
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

Server::~Server() noexcept {
  for (const GameMap::value_type& activity: _activeGames) {
    (activity.second.ptr)->stop();
    (activity.second.thread)->join();
    delete (activity.second.ptr);
  }
}

void Server::start() noexcept {
  try {
    _messageExchanger.startListening("connectClient", &Server::_connectClient, this);
    _messageExchanger.startListening("connectPlayer", &Server::_connectPlayer, this);
    _messageExchanger.startListening("disconnectPlayer", &Server::_disconnectPlayer, this);

    _messageExchanger.startListening("leaderboardRequest", &Server::_leaderboardRequest, this);
    _messageExchanger.startListening("playerInfoRequest", &Server::_playerRequest, this);
    _messageExchanger.startListening("followRequest", &Server::_manageFollow, this);

    _messageExchanger.startListening("packs", &Server::_packs, this);
    _messageExchanger.startListening("packKey", &Server::_packKey, this);

    _messageExchanger.startListening("newGame", &Server::_addNewGame, this);
    _messageExchanger.startListening("gameInput", &Server::_applyInput, this);
    _messageExchanger.startListening("stopGame", &Server::_quitGame, this);

    _messageExchanger.startListening("levelRequest", &Server::_levelRequest, this);
    _messageExchanger.startListening("rateLevel", &Server::_rateLevel, this);
    _messageExchanger.startListening("newSandbox", &Server::_addNewSandbox, this);
    _messageExchanger.startListening("sandboxEdition", &Server::_editSandbox, this);
    _messageExchanger.startListening("getLvlProgress", &Server::_getLvlProgress, this);
    _messageExchanger.startListening("stopSandbox", &Server::_quitSandbox, this);

    printf("[Server running]\n");

    pause();
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

void Server::stop() noexcept {
  try {
    _messageExchanger.stopListening("connectClient");
    _messageExchanger.stopListening("connectPlayer");
    _messageExchanger.stopListening("disconnectPlayer");

    _messageExchanger.stopListening("leaderboardRequest");
    _messageExchanger.stopListening("playerInfoRequest");
    _messageExchanger.stopListening("followRequest");

    _messageExchanger.stopListening("packs");
    _messageExchanger.stopListening("packKey");

    _messageExchanger.stopListening("newGame");
    _messageExchanger.stopListening("gameInput");
    _messageExchanger.stopListening("stopGame");

    _messageExchanger.stopListening("levelRequest");
    _messageExchanger.stopListening("rateLevel");
    _messageExchanger.stopListening("newSandbox");
    _messageExchanger.stopListening("sandboxEdition");
    _messageExchanger.stopListening("getLvlProgress");
    _messageExchanger.stopListening("stopSandbox");
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

inline Token Server::_initCommunicationToClient(const std::string& username, const std::string& gameID, const std::string& secondUsername) noexcept {
  std::string timestamp = getStrTimestamp();
  std::string sig = genSignature(username + gameID + secondUsername + timestamp);
  _messageExchanger.openChannel(sig);
  return Token(username, gameID, timestamp, sig, secondUsername);
}

void Server::_connectClient(const Handshake& msg) {
  Message<ClientInfo>* responsePtr;
  std::string username = msg.getUsername();

  try {
    if (msg.signingUp()) {
      switch (_databaseManager.signUp(username, msg.getPassword())) {
        case DatabaseManager::SUCCESS:
          break;
        case DatabaseManager::INVALID_USERNAME:
          throw Error("The given username is invalid");
        case DatabaseManager::INVALID_PASSWORD:
          throw Error("The given password is invalid");
        case DatabaseManager::USERNAME_EXISTS:
          throw Error("The given username already exists");
        default:
          throw Error("An unexpected error has occurred");
      }
    } else {
      if (!_databaseManager.signIn(username, msg.getPassword())) {
        throw Error("Wrong password or username");
      }
    }
    bool isAdmin = _databaseManager.isAdmin(username);
    responsePtr = new Message<ClientInfo>(_initCommunicationToClient(username, ""), ClientInfo(true, isAdmin));
  } catch (std::exception& err) {
    responsePtr = new Message<ClientInfo>(Token(), ClientInfo(false));
    _errorHandler.handleError(err);
  }

  // Send the response to the client
  try {
    _messageExchanger.openChannel(msg.getChannel());
    _messageExchanger.writeMessage(msg.getChannel(), *responsePtr);
    _messageExchanger.closeChannel(msg.getChannel());
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }

  delete responsePtr;
}

void Server::_connectPlayer(const Message<SYN>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  Message<ClientInfo>* responsePtr;

  SYN syn = msg.getData();
  std::string username = syn.username;

  try {
    if (token.getUsername() == username) {
      throw Error("Players have the same username");
    }

    if (syn.signup) {
      switch (_databaseManager.signUp(username, syn.password)) {
        case DatabaseManager::SUCCESS:
          break;
        case DatabaseManager::INVALID_USERNAME:
          throw Error("The given username is invalid");
        case DatabaseManager::INVALID_PASSWORD:
          throw Error("The given password is invalid");
        case DatabaseManager::USERNAME_EXISTS:
          throw Error("The given username already exists");
        default:
          throw Error("An unexpected error has occurred");
      }
    } else {
      if (!_databaseManager.signIn(username, syn.password)) {
        throw Error("Wrong password or username");
      }
    }
    bool isAdmin = _databaseManager.isAdmin(username);
    responsePtr = new Message<ClientInfo>(_initCommunicationToClient(token.getUsername(), "", username), ClientInfo(true, isAdmin));
  } catch (std::exception& err) {
    responsePtr = new Message<ClientInfo>(token, ClientInfo(false));
    _errorHandler.handleError(err);
  }

  // Send the response to the client
  try {
    _messageExchanger.writeMessage(token.getSignature(), *responsePtr);
    if (responsePtr->getData().connected) {
      _messageExchanger.closeChannel(token.getSignature());
    }
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }

  delete responsePtr;
}

void Server::_disconnectPlayer(const Message<bool>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  Message<bool> response(_initCommunicationToClient(token.getUsername()), true);

  // Send the response to the client
  try {
    _messageExchanger.writeMessage(token.getSignature(), response);
    _messageExchanger.closeChannel(token.getSignature());
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

void Server::_leaderboardRequest(const Message<LeaderboardRequest>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  const LeaderboardRequest& request = msg.getData();
  try {
    std::vector<PlayerInfo> leaderboard = {};
    // If no username has been specified
    if (request.username[0] == '\0') {
      _databaseManager.populateLeaderboard(leaderboard, request.nbEntries, request.offset);
    } else {
      _databaseManager.populateFollows(leaderboard, token.getUsername());
    }

    // Send the number of results
    _messageExchanger.writeMessage(msg.getTokenSignature(), unsigned(leaderboard.size()));

    // Send the results
    if (!leaderboard.empty()) {
      _messageExchanger.writeMessage(msg.getTokenSignature(), leaderboard);
    }
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

void Server::_playerRequest(const Message<PlayerInfoRequest>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  PlayerInfo* responsePtr;
  try {
    responsePtr = new PlayerInfo(_databaseManager.getStats(msg.getData().username, token.getUsername()));
  } catch (std::exception& err) {
    responsePtr = new PlayerInfo(PlayerInfo("", 0, 0));
    _errorHandler.handleError(err);
  }

  // Send the response to the client
  try {
    _messageExchanger.writeMessage(token.getSignature(), *responsePtr);
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }

  delete responsePtr;
}

void Server::_manageFollow(const Message<FollowRequest>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  bool success;
  try {
    if (msg.getData().add) {
      success = _databaseManager.follow(token.getUsername(), msg.getData().username);
    } else {
      success = _databaseManager.unfollow(token.getUsername(), msg.getData().username);
    }
  } catch (std::exception& err) {
    success = false;
    _errorHandler.handleError(err);
  }

  // Send the response to the client
  try {
    _messageExchanger.writeMessage(msg.getTokenSignature(), success);
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

bool Server::_gameExists(const std::string& activityID) const noexcept {
  return _activeGames.find(activityID) != _activeGames.end();
}

std::string Server::_generateGameID() const noexcept {
  std::string gameID;
  do {
    gameID = genRandomStr(ACTIVITYID_LENGTH);
  } while (_gameExists(gameID));
  return gameID;
}

void Server::_addNewGame(const Message<GameSettings>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
  }

  Message<bool>* responsePtr;

  try {
    if (!token.getActivityID().empty()) {
      throw Error("This client is already in a game");
    }
    if (token.getUsername() == token.getGuestUsername()) {
      throw Error("Usernames are the same");
    }

    // Create a new game
    std::string username = token.getUsername();
    std::string gameID = _generateGameID();
    Game* gamePtr = new Game(msg.getData(), &_databaseManager, {msg.getData().levelID});
    Token newToken = _initCommunicationToClient(username, gameID);

    _activeGames.insert({gameID, {gamePtr, newToken.getSignature(), {username, token.getGuestUsername()}}});
    std::thread* newThread = new std::thread(&Server::_playGame, this, gameID);
    (_activeGames.find(gameID)->second).thread = newThread;

    responsePtr = new Message<bool>(newToken, true);
  } catch (std::exception& err) {
    responsePtr = new Message<bool>(token, false);
    _errorHandler.handleError(err);
  }

  // Send the response to the client
  try {
    _messageExchanger.writeMessage(token.getSignature(), *responsePtr);
    if (responsePtr->getData()) {
      _messageExchanger.closeChannel(token.getSignature());
    }
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }

  delete responsePtr;
}

void Server::_playGame(const std::string& gameID) {
  Game* game;
  GameMap::iterator gameIt = _activeGames.find(gameID);
  if (gameIt == _activeGames.end() || !(game = dynamic_cast<Game*>((gameIt->second).ptr))) {
    throw Error("Error while starting a game");
  }

  try {
    std::string tokenSignature = (gameIt->second).tokenSignature;

    game->start();
    do {
      std::chrono::time_point<std::chrono::system_clock> timer_start = std::chrono::system_clock::now();

      game->refresh();

      _messageExchanger.writeMessage(tokenSignature, game->getRefreshFrame());
      std::vector<EntityFrame> entityFrames;
      game->getEntityFrames(entityFrames);
      _messageExchanger.writeMessage(tokenSignature, entityFrames);

      std::chrono::time_point<std::chrono::system_clock> timer_stop = std::chrono::system_clock::now();
      long int delta = std::chrono::duration_cast<std::chrono::microseconds>(timer_stop - timer_start).count();  // µs
      long int wait = TICK - delta;

      if (wait > 0) {
        usleep(unsigned(wait));
      }

    } while (!game->hasEnded());

    _messageExchanger.writeMessage(tokenSignature, game->getRefreshFrame());
    std::vector<EntityFrame> entityFrames;
    game->getEntityFrames(entityFrames);
    _messageExchanger.writeMessage(tokenSignature, entityFrames);

  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

void Server::_applyInput(const Message<int>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  Game* game;
  GameMap::iterator gameIt = _activeGames.find(token.getActivityID());
  if (gameIt == _activeGames.end() || !(game = dynamic_cast<Game*>((gameIt->second).ptr))) {
    _errorHandler.handleError(Error("This game does not exist"));
    return;
  }

  try {
    game->applyInput(msg.getData());
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

void Server::_quitGame(const Message<Channel>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  Activity* activityPtr;
  GameMap::iterator activityIt = _activeGames.find(token.getActivityID());
  if (activityIt == _activeGames.end() || !(activityPtr = (activityIt->second).ptr)) {
    _errorHandler.handleError(Error("This game does not exist"));
    return;
  }

  try {
    GameStatus& gameStatus = activityIt->second;

    // Update scores in database if this is a game
    Game* gamePtr = dynamic_cast<Game*>(activityPtr);
    if (!activityPtr->stopped() && gamePtr) {
      RefreshFrame refreshFrame = gamePtr->getRefreshFrame();
      _databaseManager.newScore(gameStatus.usernames[0], int(refreshFrame.score[0]));
      if (!gameStatus.usernames[1].empty()) {
        _databaseManager.newScore(gameStatus.usernames[1], int(refreshFrame.score[1]));
      }
    }

    // Send the new token to the client
    std::string responseChannel = msg.getData().channelName;
    _messageExchanger.closeChannel(gameStatus.tokenSignature);
    Token newToken = _initCommunicationToClient(gameStatus.usernames[0], "");
    _messageExchanger.openChannel(responseChannel);
    _messageExchanger.writeMessage(responseChannel, Message<bool>(newToken, true));
    _messageExchanger.closeChannel(responseChannel);

    std::thread* activityThread = gameStatus.thread;
    activityPtr->stop();
    activityThread->join();
    delete activityThread;
    delete activityPtr;
    _activeGames.erase(activityIt);
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

bool Server::_sandboxExists(const std::string& activityID) const noexcept {
  return _activeSandboxes.find(activityID) != _activeSandboxes.end();
}

std::string Server::_generateSandboxID() const noexcept {
  std::string sandboxID;
  do {
    sandboxID = genRandomStr(ACTIVITYID_LENGTH);
  } while (_sandboxExists(sandboxID));

  return sandboxID;
}

void Server::_levelRequest(const Message<LevelRequest>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
  }

  LevelRequest request = msg.getData();
  std::string user = (request.user[0] != '\0') ? request.user : "";

  try {
    std::vector<LevelInfo> levels;
    _databaseManager.getLevels(levels, request.nbEntries, request.offset, user);

    // Send number of results
    _messageExchanger.writeMessage(token.getSignature(), unsigned(levels.size()));
    // Send results
    _messageExchanger.writeMessage(token.getSignature(), levels);
  } catch (std::exception& err) {
    _messageExchanger.writeMessage(token.getSignature(), 0);
    _errorHandler.handleError(err);
  }
}

void Server::_rateLevel(const Message<LevelRate>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
  }

  LevelRate lvlRate = msg.getData();

  try {
    unsigned rating = (lvlRate.rating <= MAX_RATING) ? lvlRate.rating : MAX_RATING;
    _databaseManager.setRate(lvlRate.levelID, token.getUsername(), int(rating));
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

void Server::_addNewSandbox(const Message<SandboxSettings>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
  }

  Message<bool>* responsePtr;

  try {
    if (!token.getActivityID().empty()) {
      throw Error("This client is already in a game");
    }

    // Create a new game
    SandboxSettings sandboxSettings = msg.getData();
    Sandbox* sandboxPtr = nullptr;

    if (sandboxSettings.levelId != -1) {
      LevelInfo lvlInfo = _databaseManager.getLevelInfo(sandboxSettings.levelId);
      if (std::string(lvlInfo.creator) == token.getUsername()) {
        sandboxPtr = new Sandbox(sandboxSettings.levelId);
        std::map<unsigned, std::vector<EntityInfo>> sandboxMap;
        sandboxPtr->addEntities(_databaseManager.populateLevel(sandboxMap, sandboxSettings.levelId));
      }
    } else {
      sandboxPtr = new Sandbox(_databaseManager.addLevel(token.getUsername(), sandboxSettings.levelName));
    }

    if (sandboxPtr) {
      std::string sandboxID = _generateSandboxID();
      _activeSandboxes.insert({sandboxID, {sandboxPtr, token.getSignature(), token.getUsername()}});

      // Send response to the client
      Token newToken = _initCommunicationToClient(token.getUsername(), sandboxID);
      responsePtr = new Message<bool>(newToken, true);
    } else {
      throw Error("A user tried to modify another player's level");
    }

  } catch (std::exception& err) {
    responsePtr = new Message<bool>(token, false);
    _errorHandler.handleError(err);
  }

  // Send the response to the client
  try {
    _messageExchanger.writeMessage(token.getSignature(), *responsePtr);
    if (responsePtr->getData()) {
      _messageExchanger.closeChannel(token.getSignature());
    }
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }

  delete responsePtr;
}

void Server::_editSandbox(const Message<SandboxEdition>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  Activity* activityPtr;
  SandboxMap::iterator sandboxId = _activeSandboxes.find(token.getActivityID());
  if (sandboxId == _activeSandboxes.end() || !(activityPtr = (sandboxId->second).ptr)) {
    _errorHandler.handleError(Error("This game does not exist"));
    return;
  }

  try {
    SandboxEdition edition = msg.getData();
    Sandbox* sandboxPtr = dynamic_cast<Sandbox*>(activityPtr);
    if (edition.add) {
      sandboxPtr->addEntity(edition.progress, edition.entityInfo);
      _databaseManager.addLevelEntity(sandboxPtr->getId(), edition.progress, edition.entityInfo);
    } else {
      sandboxPtr->delEntity(edition.progress, edition.entityInfo);
      _databaseManager.removeLevelEntity(sandboxPtr->getId(), edition.progress, edition.entityInfo);
    }
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

void Server::_quitSandbox(const Message<Channel>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  Activity* activityPtr;
  SandboxMap::iterator sandboxId = _activeSandboxes.find(token.getActivityID());
  if (sandboxId == _activeSandboxes.end() || !(activityPtr = (sandboxId->second).ptr)) {
    _errorHandler.handleError(Error("This game does not exist"));
    return;
  }

  try {
    SandboxStatus sandboxStatus = sandboxId->second;

    // Send the new token to the client
    std::string responseChannel = msg.getData().channelName;
    _messageExchanger.closeChannel(sandboxStatus.tokenSignature);
    Token newToken = _initCommunicationToClient(sandboxStatus.username, "");
    _messageExchanger.openChannel(responseChannel);
    _messageExchanger.writeMessage(responseChannel, Message<bool>(newToken, true));
    _messageExchanger.closeChannel(responseChannel);

    activityPtr->stop();
    delete activityPtr;
    _activeSandboxes.erase(sandboxId);
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

void Server::_getLvlProgress(const Message<unsigned>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  Activity* activityPtr;
  SandboxMap::iterator sandboxId = _activeSandboxes.find(token.getActivityID());
  if (sandboxId == _activeSandboxes.end() || !(activityPtr = (sandboxId->second).ptr)) {
    _errorHandler.handleError(Error("This game does not exist"));
    return;
  }

  try {
    Sandbox* sbPtr = dynamic_cast<Sandbox*>(activityPtr);
    std::vector<EntityInfo> entities;
    _databaseManager.populateLevel(entities, sbPtr->getId(), msg.getData());
    _messageExchanger.writeMessage(token.getSignature(), unsigned(entities.size()));
    _messageExchanger.writeMessage(token.getSignature(), entities);
  } catch (std::exception& err) {
    _messageExchanger.writeMessage(token.getSignature(), 0);
    _errorHandler.handleError(err);
  }
}

void Server::_packs(const Message<PlayerInfoRequest>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  try {
    std::vector<Pack> packs = {};
    _databaseManager.populatePacks(packs, msg.getData().username);

    // Send the number of results
    _messageExchanger.writeMessage(msg.getTokenSignature(), unsigned(packs.size()));

    // Send the results
    if (!packs.empty()) {
      _messageExchanger.writeMessage(msg.getTokenSignature(), packs);
    }
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}

void Server::_packKey(const Message<PackKeyRequest>& msg) {
  Token token = msg.getToken();
  if (!_isTokenValid(token)) {
    _errorHandler.handleError(Error("Invalid token"));
    return;
  }

  switch (msg.getData().command) {
    // Use
    case 0: {
      bool activated = _databaseManager.usePackKey(msg.getData().key, msg.getData().username);
      _messageExchanger.writeMessage(msg.getTokenSignature(), activated);
    } break;

    // List
    case 1:
      if (_databaseManager.isAdmin(token.getUsername())) {
        _getPackKeys(msg);
      }
      break;

    // Add
    case 2:
      if (_databaseManager.isAdmin(token.getUsername())) {
        PackKey sk = _databaseManager.addPackKey(msg.getData().pack, msg.getData().key, msg.getData().uses);
        _messageExchanger.writeMessage(msg.getTokenSignature(), sk);
      }
      break;

    // Remove
    case 3:
      if (_databaseManager.isAdmin(token.getUsername())) {
        _databaseManager.removePackKey(msg.getData().key);
      }
      break;
  }
}

void Server::_getPackKeys(const Message<PackKeyRequest>& msg) {
  try {
    std::vector<PackKey> packKeys = {};
    _databaseManager.populatePackKey(packKeys);

    // Send the number of results
    _messageExchanger.writeMessage(msg.getTokenSignature(), unsigned(packKeys.size()));

    // Send the results
    if (!packKeys.empty()) {
      _messageExchanger.writeMessage(msg.getTokenSignature(), packKeys);
    }
  } catch (std::exception& err) {
    _errorHandler.handleError(err);
  }
}
