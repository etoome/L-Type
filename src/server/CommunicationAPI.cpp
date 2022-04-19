/******************************************************************************
* File:             CommunicationAPI.cpp
*
* Author:           Tanguy Lissenko, Quentin Magron
* Created:          21-02-18
* Description:
*****************************************************************************/

#include "server/CommunicationAPI.hpp"

#include "Error.hpp"
#include "Message.hpp"
#include "server/MessageExchanger.hpp"

/* Global variable: prevent the user of the `CommunicationAPI` to access the `MessageExchanger`
 *  class from the header.
 */
MessageExchanger messageExchanger;

CommunicationAPI::CommunicationAPI() noexcept: _channel(std::to_string(getpid())) {
  messageExchanger.openChannel(_channel);

  messageExchanger.openChannel("connectClient");
  messageExchanger.openChannel("connectPlayer");
  messageExchanger.openChannel("disconnectPlayer");

  messageExchanger.openChannel("leaderboardRequest");
  messageExchanger.openChannel("playerInfoRequest");
  messageExchanger.openChannel("followRequest");

  messageExchanger.openChannel("packs");
  messageExchanger.openChannel("packKey");

  messageExchanger.openChannel("newGame");
  messageExchanger.openChannel("gameInput");
  messageExchanger.openChannel("stopGame");

  messageExchanger.openChannel("levelRequest");
  messageExchanger.openChannel("rateLevel");
  messageExchanger.openChannel("newSandbox");
  messageExchanger.openChannel("sandboxEdition");
  messageExchanger.openChannel("getLvlProgress");
  messageExchanger.openChannel("stopSandbox");
}

CommunicationAPI::~CommunicationAPI() noexcept {
  messageExchanger.closeChannel(_channel);

  messageExchanger.closeChannel("connectClient");
  messageExchanger.closeChannel("connectPlayer");
  messageExchanger.closeChannel("disconnectPlayer");

  messageExchanger.closeChannel("leaderboardRequest");
  messageExchanger.closeChannel("playerInfoRequest");
  messageExchanger.closeChannel("followRequest");

  messageExchanger.closeChannel("packs");
  messageExchanger.closeChannel("packKey");

  messageExchanger.closeChannel("newGame");
  messageExchanger.closeChannel("gameInput");
  messageExchanger.closeChannel("stopGame");

  messageExchanger.closeChannel("levelRequest");
  messageExchanger.closeChannel("rateLevel");
  messageExchanger.closeChannel("newSandbox");
  messageExchanger.closeChannel("sandboxEdition");
  messageExchanger.closeChannel("getLvlProgress");
  messageExchanger.closeChannel("stopSandbox");
}

std::string CommunicationAPI::getUsername() const noexcept {
  return _token.getUsername();
}

std::string CommunicationAPI::getGuestUsername() const noexcept {
  return _token.getGuestUsername();
}

bool CommunicationAPI::isAdmin() const noexcept {
  return _isAdmin;
}

template<typename Data>
Data CommunicationAPI::_read(std::size_t nData) const {
  Data* dataPtr = static_cast<Data*>(malloc(sizeof(Data) * nData));

  if (!messageExchanger.readMessage(dataPtr, _channel, nData)) {
    free(dataPtr);
    throw FatalError("Could not communicate with the server");
  }

  Data returnValue = *dataPtr;
  free(dataPtr);
  return returnValue;
}

template<typename Data>
std::vector<Data>& CommunicationAPI::_read(std::vector<Data>& dest, std::size_t nData) const {
  Data* dataPtr = static_cast<Data*>(malloc(sizeof(Data) * nData));

  if (!messageExchanger.readMessage(dataPtr, _channel, nData)) {
    free(dataPtr);
    throw FatalError("Could not communicate with the server");
  }

  for (std::size_t d = 0; d != nData; ++d) {
    dest.push_back(*(dataPtr + d));
  }

  free(dataPtr);
  return dest;
}

ClientInfo CommunicationAPI::_login(const std::string& username, const std::string& password, bool signup) {
  if (_secondPlayer) {
    return ClientInfo(false);
  }

  if (_token.isEmpty()) {
    messageExchanger.writeMessage("connectClient", Handshake(SYN{username, password, _channel, signup}));
  } else {
    messageExchanger.writeMessage("connectPlayer", Message<SYN>(_token, {username, password, _channel, signup}));
  }

  using Response = Message<ClientInfo>;
  Response response = _read<Response>();

  if (response.getData().connected) {
    if (!_token.isEmpty()) {
      _secondPlayer = true;
    } else {
      _isAdmin = response.getData().admin;
    }

    _token = response.getToken();
    messageExchanger.closeChannel(_channel);
    _channel = _token.getSignature();
    messageExchanger.openChannel(_channel);
  }

  return response.getData();
}

ClientInfo CommunicationAPI::signIn(const std::string& username, const std::string& password) {
  return _login(username, password, false);
}

ClientInfo CommunicationAPI::signUp(const std::string& username, const std::string& password) {
  return _login(username, password, true);
}

void CommunicationAPI::signOut() noexcept {
  _token = Token();
  _channel = std::to_string(getpid());
}

void CommunicationAPI::removeSecondPlayer() {
  if (!_secondPlayer) {
    return;
  }

  messageExchanger.writeMessage("disconnectPlayer", Message<bool>(_token, true));

  using Response = Message<bool>;
  Response response = _read<Response>();

  if (response.getData()) {
    _secondPlayer = true;
    _token = response.getToken();
    messageExchanger.closeChannel(_channel);
    _channel = _token.getSignature();
    messageExchanger.openChannel(_channel);
  }
}

std::vector<PlayerInfo>& CommunicationAPI::_getLeaderboard(std::vector<PlayerInfo>& dest, const std::string& username, int nbEntries, int offset) const {
  if (_token.isEmpty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("leaderboardRequest", Message<LeaderboardRequest>(_token, {username, nbEntries, offset}));

  unsigned nData = _read<unsigned int>();
  if (nData != 0) {
    _read<PlayerInfo>(dest, nData);
  }

  return dest;
}

std::vector<PlayerInfo>& CommunicationAPI::getLeaderboard(std::vector<PlayerInfo>& dest, int nbEntries, int offset) const {
  return _getLeaderboard(dest, "", nbEntries, offset);
}

std::vector<PlayerInfo>& CommunicationAPI::getFollows(std::vector<PlayerInfo>& dest, const std::string& username) const {
  return _getLeaderboard(dest, username, 0, 0);
}

PlayerInfo CommunicationAPI::getPlayerInfo(const std::string& username) const {
  if (_token.isEmpty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("playerInfoRequest", Message<PlayerInfoRequest>(_token, {username}));

  return _read<PlayerInfo>();
}

bool CommunicationAPI::_manageFollow(const std::string& username, bool add) const {
  if (_token.isEmpty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("followRequest", Message<FollowRequest>(_token, {username, add}));

  return _read<bool>();
}

bool CommunicationAPI::follow(const std::string& username) const {
  return _manageFollow(username, true);
}

bool CommunicationAPI::unfollow(const std::string& username) const {
  return _manageFollow(username, false);
}

bool CommunicationAPI::createGame(const GameSettings& settings) {
  if (_token.isEmpty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("newGame", Message<GameSettings>(_token, settings));

  using Response = Message<bool>;
  Response response = _read<Response>();

  if (response.getData()) {
    _token = response.getToken();
    messageExchanger.closeChannel(_channel);
    _channel = _token.getSignature();
    messageExchanger.openChannel(_channel);
  }

  return response.getData();
}

void CommunicationAPI::sendGameInput(int key) const {
  if (_token.getActivityID().empty()) {
    throw FatalError("Not connected to a game");
  }

  messageExchanger.writeMessage("gameInput", Message<int>(_token, key));
}

RefreshFrame CommunicationAPI::getGameState(std::vector<EntityFrame>& dest) const {
  if (_token.getActivityID().empty()) {
    throw FatalError("Not connected");
  }

  RefreshFrame gameState = _read<RefreshFrame>();

  std::size_t nData = gameState.nbEntities;
  if (nData != 0) {
    _read<EntityFrame>(dest, nData);
  }

  return gameState;
}

void CommunicationAPI::quitGame() {
  if (_token.getActivityID().empty()) {
    throw FatalError("Not connected to a game");
  }

  std::string responseChannel = std::to_string(getpid());
  messageExchanger.closeChannel(_channel);
  _channel = responseChannel;
  messageExchanger.openChannel(responseChannel);
  messageExchanger.writeMessage("stopGame", Message<Channel>(_token, {responseChannel}));

  using Response = Message<bool>;
  Response response = _read<Response>();
  messageExchanger.closeChannel(responseChannel);

  _token = response.getToken();
  _channel = _token.getSignature();
  messageExchanger.openChannel(_channel);

  _secondPlayer = false;
}

void CommunicationAPI::rateLevel(int lvlID, unsigned rating) const {
  if (_token.isEmpty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("rateLevel", Message<LevelRate>(_token, {lvlID, rating}));
}

std::vector<LevelInfo>& CommunicationAPI::getLevels(std::vector<LevelInfo>& dest, const std::string& username, int nbEntries, int offset) const {
  if (_token.isEmpty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("levelRequest", Message<LevelRequest>(_token, {username.c_str(), nbEntries, offset}));

  unsigned nLevels = _read<unsigned>();

  if (nLevels != 0) {
    _read<LevelInfo>(dest, nLevels);
  }

  return dest;
}

bool CommunicationAPI::createSandbox(const SandboxSettings& settings) {
  if (!_token.getActivityID().empty()) {
    throw FatalError("An activity is already started");
  }

  messageExchanger.writeMessage("newSandbox", Message<SandboxSettings>(_token, settings));

  using Response = Message<bool>;
  Response response = _read<Response>();

  if (response.getData()) {
    _token = response.getToken();
    messageExchanger.closeChannel(_channel);
    _channel = _token.getSignature();
    messageExchanger.openChannel(_channel);
  }

  return response.getData();
}

void CommunicationAPI::sendSandboxEdition(const SandboxEdition& edition) const {
  if (_token.getActivityID().empty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("sandboxEdition", Message<SandboxEdition>(_token, edition));
}

std::vector<EntityInfo>& CommunicationAPI::getLvlProgress(std::vector<EntityInfo>& dest, unsigned progress) const {
  if (_token.getActivityID().empty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("getLvlProgress", Message<unsigned>(_token, progress));

  unsigned nData = _read<unsigned>();
  if (nData != 0) {
    _read<EntityInfo>(dest, std::size_t(nData));
  }

  return dest;
}

void CommunicationAPI::quitSandbox() {
  if (_token.getActivityID().empty()) {
    throw FatalError("Not connected");
  }

  std::string responseChannel = std::to_string(getpid());
  messageExchanger.closeChannel(_channel);
  _channel = responseChannel;
  messageExchanger.openChannel(responseChannel);
  messageExchanger.writeMessage("stopSandbox", Message<Channel>(_token, {responseChannel}));

  using Response = Message<bool>;
  Response response = _read<Response>();
  messageExchanger.closeChannel(responseChannel);

  _token = response.getToken();
  _channel = _token.getSignature();
  messageExchanger.openChannel(_channel);
}

std::vector<Pack>& CommunicationAPI::getPacks(std::vector<Pack>& dest, const std::string& username) {
  if (_token.isEmpty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("packs", Message<PlayerInfoRequest>(_token, PlayerInfoRequest(username)));

  unsigned nData = _read<unsigned int>();
  if (nData != 0) {
    _read<Pack>(dest, nData);
  }

  return dest;
}

bool CommunicationAPI::activatePack(const std::string& key, const std::string& username) {
  if (_token.isEmpty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("packKey", Message<PackKeyRequest>(_token, PackKeyRequest(0, key, "", 1, username)));

  return _read<bool>();
}

std::vector<PackKey>& CommunicationAPI::getPackKeys(std::vector<PackKey>& dest) {
  if (_token.isEmpty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("packKey", Message<PackKeyRequest>(_token, PackKeyRequest(1)));

  unsigned nData = _read<unsigned int>();
  if (nData != 0) {
    _read<PackKey>(dest, nData);
  }

  return dest;
}

std::string CommunicationAPI::addPackKey(const std::string& pack, const std::string& key, int uses) {
  if (_token.isEmpty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("packKey", Message<PackKeyRequest>(_token, PackKeyRequest(2, key, pack, uses)));

  PackKey response = _read<PackKey>();

  return response.key;
}

void CommunicationAPI::removePackKey(const std::string& key) {
  if (_token.isEmpty()) {
    throw FatalError("Not connected");
  }

  messageExchanger.writeMessage("packKey", Message<PackKeyRequest>(_token, PackKeyRequest(3, key)));
}
