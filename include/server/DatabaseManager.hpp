#pragma once

#include <sqlite3.h>

#include <map>
#include <string>
#include <vector>

#include "EntityInfo.hpp"
#include "MessageData.hpp"

class DatabaseManager {
 private:
  sqlite3* _db = nullptr;

  /* Return true if the operation was successful.
   */
  template<typename FirstArg, typename... Args>
  bool _bindData(sqlite3_stmt* stmt, const FirstArg& firstData, const Args&... otherData) const noexcept;

  bool _userExists(const std::string& username) const;

  std::string _getPassword(const std::string& username) const;

  bool _isFollowing(const std::string& follower, const std::string& followed);
  void _follow(const std::string& follower, const std::string& followed);
  void _unfollow(const std::string& follower, const std::string& followed);

  void _updateBestScore(const std::string& username, int score);
  void _updateXP(const std::string& username, int score);

  int _packId(const std::string& name);
  bool _packKeyExists(const std::string& key);

  void _decrementUses(const std::string& key);
  void _addPackAccount(const std::string& username, int pack);

  int _keyToPack(const std::string& key);

 public:
  static const int SUCCESS = 0;
  static const int INVALID_USERNAME = 1;
  static const int INVALID_PASSWORD = 2;
  static const int USERNAME_EXISTS = 3;

  DatabaseManager() = delete;
  DatabaseManager(const std::string& dbPath);
  ~DatabaseManager() noexcept;
  DatabaseManager(const DatabaseManager&) = delete;
  DatabaseManager& operator=(const DatabaseManager&) = delete;

  bool isAdmin(std::string username) const;

  std::vector<PlayerInfo>& populateLeaderboard(std::vector<PlayerInfo>& leaderboard, int size, int offset = 0);

  std::vector<PlayerInfo>& populateFollows(std::vector<PlayerInfo>& follows, const std::string& username);
  bool follow(const std::string& username, const std::string& toFollow);
  bool unfollow(const std::string& username, const std::string& toFollow);

  PlayerInfo getStats(const std::string& username, const std::string& askingUser = "");

  /* The return value is one of the following:
   *  - SUCCESS
   *  - INVALID_USERNAME
   *  - INVALID_PASSWORD
   *  - USERNAME_EXISTS
   */
  int signUp(const std::string& username, const std::string& password);
  bool signIn(const std::string& username, const std::string& password) const;

  void newScore(const std::string& username, int score);

  std::vector<Pack>& populatePacks(std::vector<Pack>& packs, const std::string& username);

  std::vector<PackKey>& populatePackKey(std::vector<PackKey>& packKeys);
  PackKey addPackKey(const std::string& pack, const std::string& key, int uses);
  bool usePackKey(const std::string& key, const std::string& username);
  void removePackKey(const std::string& key = std::string());

  LevelInfo getLevelInfo(int id) const;
  std::vector<LevelInfo>& getLevels(std::vector<LevelInfo>& dest, int nbEntries = -1, int offset = 0, const std::string& username = "");
  std::map<unsigned, std::vector<EntityInfo>>& populateLevel(std::map<unsigned, std::vector<EntityInfo>>&, int id);
  std::vector<EntityInfo>& populateLevel(std::vector<EntityInfo>&, int id, unsigned progress);
  void addLevelEntity(int levelId, unsigned progress, const EntityInfo& entity);
  void removeLevelEntity(int levelId, unsigned progress, const EntityInfo& entity);
  int addLevel(const std::string& username, const std::string& levelName);

  void removeLevel(int id);
  void setRate(int levelId, const std::string& username, int rate);
};
