#pragma once

#include <string>
#include <vector>

#include "EntityInfo.hpp"
#include "GameSettings.hpp"
#include "MessageData.hpp"
#include "SandboxEdition.hpp"
#include "SandboxSettings.hpp"
#include "Token.hpp"

class CommunicationAPI {
 private:
  Token _token = Token();
  std::string _channel;
  bool _secondPlayer = false;
  bool _isAdmin = false;

  template<typename Data>
  Data _read(std::size_t nData = 1) const;
  template<typename Data>
  std::vector<Data>& _read(std::vector<Data>& dest, std::size_t nData = 1) const;

  ClientInfo _login(const std::string& username, const std::string& password, bool signup);
  std::vector<PlayerInfo>& _getLeaderboard(std::vector<PlayerInfo>& dest, const std::string& username, int nbEntries, int offset = 0) const;
  bool _manageFollow(const std::string& username, bool add) const;

 public:
  CommunicationAPI() noexcept;
  ~CommunicationAPI() noexcept;

  std::string getUsername() const noexcept;
  std::string getGuestUsername() const noexcept;
  bool isAdmin() const noexcept;

  ClientInfo signIn(const std::string& username, const std::string& password);
  ClientInfo signUp(const std::string& username, const std::string& password);
  void signOut() noexcept;

  void removeSecondPlayer();

  std::vector<PlayerInfo>& getLeaderboard(std::vector<PlayerInfo>& dest, int nbEntries, int offset = 0) const;
  std::vector<PlayerInfo>& getFollows(std::vector<PlayerInfo>& dest, const std::string& username) const;

  PlayerInfo getPlayerInfo(const std::string& username) const;
  bool follow(const std::string& username) const;
  bool unfollow(const std::string& username) const;

  bool createGame(const GameSettings&);
  void sendGameInput(int key) const;
  RefreshFrame getGameState(std::vector<EntityFrame>& dest) const;
  void quitGame();

  void rateLevel(int lvlID, unsigned rating) const;
  std::vector<LevelInfo>& getLevels(std::vector<LevelInfo>& dest, const std::string& username, int nbEntries, int offset = 0) const;

  bool createSandbox(const SandboxSettings&);
  void sendSandboxEdition(const SandboxEdition&) const;
  std::vector<EntityInfo>& getLvlProgress(std::vector<EntityInfo>& dest, unsigned progress) const;
  void quitSandbox();

  std::vector<Pack>& getPacks(std::vector<Pack>& dest, const std::string& username);
  bool activatePack(const std::string& key, const std::string& username);
  std::vector<PackKey>& getPackKeys(std::vector<PackKey>& dest);
  std::string addPackKey(const std::string& pack, const std::string& key, int uses);
  void removePackKey(const std::string& key);
};
