#pragma once

#include <map>
#include <string>
#include <thread>

#include "ErrorHandler.hpp"
#include "GameSettings.hpp"
#include "Message.hpp"
#include "MessageData.hpp"
#include "SandboxEdition.hpp"
#include "SandboxSettings.hpp"
#include "Token.hpp"
#include "server/DatabaseManager.hpp"
#include "server/MessageExchanger.hpp"
#include "server/game/Game.hpp"
#include "server/sandbox/Sandbox.hpp"

/* The main server class.
 * Can read messages from the client on channels.
 * Can respond to this message on the client channel.
 */
class Server final {
 private:
  struct GameStatus {
    Activity* ptr;
    std::string tokenSignature;
    std::string usernames[2];
    std::thread* thread = nullptr;
  };
  using GameMap = std::map<const std::string, GameStatus>;

  struct SandboxStatus {
    Activity* ptr;
    std::string tokenSignature;
    std::string username;
  };
  using SandboxMap = std::map<const std::string, SandboxStatus>;

  static inline bool _isTokenValid(const Token& token) noexcept;

  ErrorHandler _errorHandler;
  DatabaseManager _databaseManager;
  MessageExchanger _messageExchanger = {};

  GameMap _activeGames = {};
  SandboxMap _activeSandboxes = {};

  /* Create a communication channel to the client.
  *  Return an access token.
   */
  Token _initCommunicationToClient(const std::string& username, const std::string& gameID = "", const std::string& secondUsername = "") noexcept;

  /* Check if the given username and password match.
   * Will send a response to the client: Message<bool> with a completed
   *  token and a boolean indicating if the client has been connected or not.
   */
  void _connectClient(const Handshake&);

  /* Check if the given username and password match.
   * Will send a response to the client: Message<bool> with a completed
   *  token and a boolean indicating if the client has been connected or not.
   */
  void _connectPlayer(const Message<SYN>&);

  /* Remove the second player from the token.
   * Will send a responde to the client: Message<bool> with an updated
   *  token and a boolean indicating if the operation was successful.
   */
  void _disconnectPlayer(const Message<bool>&);

  /* Read the leaderboard from the database.
   * Will send a response to the client: unsigned with the number of
   *  results to read, then send as many PlayerInfo than results.
   */
  void _leaderboardRequest(const Message<LeaderboardRequest>&);

  /* Read the player info in the database.
   * Will send a response to the client: PlayerInfo
   */
  void _playerRequest(const Message<PlayerInfoRequest>&);

  /* Start or stop following another user.
   * Will send a response to the client: bool with a boolean
   *  indicating if the operation was successful.
   */
  void _manageFollow(const Message<FollowRequest>&);

  /* Check if the game exists.
   */
  bool _gameExists(const std::string& activityID) const noexcept;

  /* Generate a unique game id.
   */
  std::string _generateGameID() const noexcept;

  /* Create a new game and start it.
   * Will send a response to the client: Message<bool> with a completed
   *  token and a boolean indicating if the game has been created or not.
   */
  void _addNewGame(const Message<GameSettings>&);

  /* Apply a client input on its related game.
   */
  void _applyInput(const Message<int>&);

  void _playGame(const std::string& gameID);
  void _quitGame(const Message<Channel>&);

  /* Check if the sandbox exists.
   */
  bool _sandboxExists(const std::string& activityID) const noexcept;

  /* Generate a unique sandbox id.
   */
  std::string _generateSandboxID() const noexcept;

  /* Get levels created by the player whose username is in the message.
   * An empty username remove this filter.
   * Will send a response to the client: unsigned with the number of
   *  levels, then send as many LevelInfo than levels.
   */
  void _levelRequest(const Message<LevelRequest>&);

  /* Rate a level.
   */
  void _rateLevel(const Message<LevelRate>&);

  /* Create a new Sandbox and start it.
   * Will send a response to the client: Message<bool> with a completed
   *  token and a boolean indicating if the sandbox has been created or not.
   */
  void _addNewSandbox(const Message<SandboxSettings>&);

  /* Edit an existing sandbox.
   */
  void _editSandbox(const Message<SandboxEdition>&);

  /* Get entities at a specific progress in the level.
   * Will send a response to the client: unsigned with the number of
   *  entities, then send as many EntityInfo than entities.
   */
  void _getLvlProgress(const Message<unsigned>&);

  void _quitSandbox(const Message<Channel>&);

  /* Get existing packs with a flag indicating if the user
   *  already own it or not.
   * Will send a response to the client: unsigned with the number of
   *  packs, then send as many Pack than packs.
   */
  void _packs(const Message<PlayerInfoRequest>&);

  /* Operations on packs.
   */
  void _packKey(const Message<PackKeyRequest>&);

  /* Get existing packs keys.
   * Will send a response to the client: unsigned with the number of
   *  keys, then send as many PackKey than keys.
   */
  void _getPackKeys(const Message<PackKeyRequest>&);

 public:
  Server() noexcept;
  ~Server() noexcept;

  /* Start the server.
   * Will start listening on these channels:
   *  - connectClient
   *  - connectPlayer
   *  - disconnectPlayer
   *  - leaderboardRequest
   *  - playerInfoRequest
   *  - followRequest
   *  - packs
   *  - packKey
   *  - newGame
   *  - gameInput
   *  - stopGame
   *  - levelRequest
   *  - rateLevel
   *  - newSandbox
   *  - sandboxEdition
   *  - getLvlProgress
   *  - stopSandbox
   */
  void start() noexcept;

  /* Stop the server.
   */
  void stop() noexcept;
};
