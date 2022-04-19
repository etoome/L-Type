#include "server/DatabaseManager.hpp"

#include <sqlite3.h>
#include <sys/stat.h>

#include <fstream>

#include "Error.hpp"
#include "constants.hpp"
#include "server/utils.hpp"
#include "utils.hpp"

template<typename FirstArg, typename... Args>
bool bindToStmt(sqlite3_stmt*, unsigned int, const FirstArg&, const Args&...) noexcept;

template<>
bool bindToStmt<std::string>(sqlite3_stmt* stmt, unsigned int depth, const std::string& data) noexcept {
  return sqlite3_bind_text(stmt, int(depth), data.c_str(), -1, SQLITE_STATIC) == SQLITE_OK;
}

template<>
bool bindToStmt<int>(sqlite3_stmt* stmt, unsigned int depth, const int& data) noexcept {
  return sqlite3_bind_int(stmt, int(depth), data) == SQLITE_OK;
}

template<typename FirstArg, typename... Args>
bool bindToStmt(sqlite3_stmt* stmt, unsigned int depth, const FirstArg& firstData, const Args&... otherData) noexcept {
  if (!bindToStmt(stmt, depth, firstData)) {
    return false;
  }
  return bindToStmt(stmt, depth + 1, otherData...);
}

DatabaseManager::DatabaseManager(const std::string& dbPath) {
  struct stat buffer;
  bool newDatabase = stat(dbPath.c_str(), &buffer) != 0;

  if (sqlite3_open(dbPath.c_str(), &_db) != SQLITE_OK) {
    throw FatalError("Could not open the database");
  }

  if (!newDatabase) return;
}

DatabaseManager::~DatabaseManager() noexcept {
  sqlite3_close(_db);
}

template<typename FirstArg, typename... Args>
bool DatabaseManager::_bindData(sqlite3_stmt* stmt, const FirstArg& firstData, const Args&... otherData) const noexcept {
  return bindToStmt(stmt, 1, firstData, otherData...);
}

bool DatabaseManager::_userExists(const std::string& username) const {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT 1 FROM accounts WHERE username = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, username)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  if (rc == SQLITE_DONE) {
    return false;
  } else if (rc != SQLITE_ROW) {
    throw Error(sqlite3_errmsg(_db));
  }

  return true;
}

std::string DatabaseManager::_getPassword(const std::string& username) const {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT password FROM accounts WHERE username = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, username)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE) {
    sqlite3_finalize(stmt);
    return "";
  } else if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  std::string password(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
  sqlite3_finalize(stmt);
  return password;
}

bool DatabaseManager::_isFollowing(const std::string& follower, const std::string& followed) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT 1 FROM follow WHERE follower = ? AND followed = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, follower, followed)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  if (rc == SQLITE_DONE) {
    return false;
  } else if (rc != SQLITE_ROW) {
    throw Error(sqlite3_errmsg(_db));
  }

  return true;
}

void DatabaseManager::_follow(const std::string& follower, const std::string& followed) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "INSERT INTO follow (follower, followed) VALUES (?, ?)";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, follower, followed)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  sqlite3_finalize(stmt);
}

void DatabaseManager::_unfollow(const std::string& follower, const std::string& followed) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "DELETE FROM follow WHERE follower = ? AND followed = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, follower, followed)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

void DatabaseManager::_updateBestScore(const std::string& username, int score) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "UPDATE leaderboard SET best_score = CASE WHEN best_score < ? THEN ? ELSE best_score END WHERE username = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, score, score, username)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  sqlite3_finalize(stmt);
}

void DatabaseManager::_updateXP(const std::string& username, int score) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "UPDATE leaderboard SET xp = xp + ? WHERE username = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, score, username)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  sqlite3_finalize(stmt);
}

int DatabaseManager::_packId(const std::string& name) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT id FROM pack WHERE name = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, name)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE) {
    return -1;
  } else if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int id = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return id;
}

bool DatabaseManager::_packKeyExists(const std::string& key) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT 1 FROM pack_key WHERE key = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, key)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  if (rc == SQLITE_DONE) {
    return false;
  } else if (rc != SQLITE_ROW) {
    throw Error(sqlite3_errmsg(_db));
  }

  return true;
}

void DatabaseManager::_decrementUses(const std::string& key) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "UPDATE pack_key SET uses = uses - 1 WHERE key = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, key)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

void DatabaseManager::_addPackAccount(const std::string& username, int pack) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "INSERT INTO account_pack (account, pack) VALUES (?, ?)";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, username, pack)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  sqlite3_finalize(stmt);
}

int DatabaseManager::_keyToPack(const std::string& key) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT pack FROM pack_key WHERE key = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, key)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc = sqlite3_step(stmt);
  if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int pack = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);
  return pack;
}

bool DatabaseManager::isAdmin(std::string username) const {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT admin FROM accounts WHERE username = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, username)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE) {
    sqlite3_finalize(stmt);
    return "";
  } else if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  bool admin = sqlite3_column_int(stmt, 0) == 1 ? true : false;

  sqlite3_finalize(stmt);
  return admin;
}

std::vector<PlayerInfo>& DatabaseManager::populateLeaderboard(std::vector<PlayerInfo>& leaderboard, int size, int offset) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT accounts.username, best_score, xp FROM leaderboard LEFT JOIN accounts ON leaderboard.username=accounts.username ORDER BY best_score DESC LIMIT ? OFFSET ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, size, offset)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc;
  while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    leaderboard.push_back({reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 2)});
  }

  sqlite3_finalize(stmt);
  return leaderboard;
}

std::vector<PlayerInfo>& DatabaseManager::populateFollows(std::vector<PlayerInfo>& follows, const std::string& username) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT followed FROM follow INNER JOIN accounts ON follow.followed=accounts.username AND follow.follower = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    std::string a = sqlite3_errmsg(_db);
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, username)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc;
  while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    const char* followed = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

    bool isFollowed = _isFollowing(followed, username);

    follows.push_back({followed, 0, 0, true, isFollowed});
  }

  sqlite3_finalize(stmt);
  return follows;
}

PlayerInfo DatabaseManager::getStats(const std::string& username, const std::string& askingUser) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT accounts.username, best_score, xp FROM leaderboard LEFT JOIN accounts ON leaderboard.username=accounts.username AND accounts.username = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, username)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_DONE) {
    return PlayerInfo{"", 0, 0};
  } else if (rc != SQLITE_ROW) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  PlayerInfo player{reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), sqlite3_column_int(stmt, 1), sqlite3_column_int(stmt, 2), _isFollowing(askingUser, username), _isFollowing(username, askingUser)};
  sqlite3_finalize(stmt);
  return player;
}

bool DatabaseManager::follow(const std::string& username, const std::string& toFollow) {
  if (toFollow == username) return false;
  if (!_userExists(toFollow)) return false;
  if (_isFollowing(username, toFollow)) return false;

  _follow(username, toFollow);

  return true;
}

bool DatabaseManager::unfollow(const std::string& username, const std::string& toFollow) {
  if (_isFollowing(username, toFollow)) {
    _unfollow(username, toFollow);

    return true;
  }

  return false;
}

int DatabaseManager::signUp(const std::string& username, const std::string& password) {
  if (username.length() < USERNAME_MIN || username.length() > USERNAME_MAX || !VALID_STR(username)) {
    return INVALID_USERNAME;
  }

  if (password.length() < PASSWORD_MIN || !VALID_STR(password)) {
    return INVALID_PASSWORD;
  }

  if (_userExists(username)) {
    return USERNAME_EXISTS;
  }

  {
    sqlite3_stmt* stmt;

    std::string sqlQuery = "INSERT INTO accounts (username, password) VALUES (?, ?)";
    if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
      throw Error(sqlite3_errmsg(_db));
    }

    std::string hashedPassword = hash(password);
    if (!_bindData(stmt, username, hashedPassword)) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    sqlite3_finalize(stmt);
  }

  {
    sqlite3_stmt* stmt;

    std::string sqlQuery = "INSERT INTO leaderboard (username) VALUES (?)";
    if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
      throw Error(sqlite3_errmsg(_db));
    }

    if (!_bindData(stmt, username)) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    sqlite3_finalize(stmt);
  }

  return SUCCESS;
}

bool DatabaseManager::signIn(const std::string& username, const std::string& password) const {
  return hash(password) == _getPassword(username);
}

void DatabaseManager::newScore(const std::string& username, int score) {
  _updateXP(username, score);
  _updateBestScore(username, score);
}

std::vector<Pack>& DatabaseManager::populatePacks(std::vector<Pack>& packs, const std::string& username) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT pack.id, pack.name, owner.owned FROM pack LEFT OUTER JOIN (SELECT pack, 1 AS owned FROM account_pack WHERE account = ?) owner ON owner.pack=pack.id";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, username)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc;
  while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    packs.push_back({
        sqlite3_column_int(stmt, 0),
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
        sqlite3_column_int(stmt, 2) == 1 ? true : false,
    });
  }

  sqlite3_finalize(stmt);
  return packs;
}

std::vector<PackKey>& DatabaseManager::populatePackKey(std::vector<PackKey>& packKeys) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT pack, key, uses FROM pack_key";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  int rc;
  while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    packKeys.push_back({reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)), sqlite3_column_int(stmt, 2)});
  }

  sqlite3_finalize(stmt);
  return packKeys;
}

PackKey DatabaseManager::addPackKey(const std::string& pack, const std::string& k, int uses) {
  std::string key;
  if (k.empty()) {
    do {
      key = genRandomStr(5);
    } while (_packKeyExists(key));
  } else {
    if (_packKeyExists(k)) {
      return PackKey();
    }
    key = k;
  }

  int packId = _packId(pack);
  if (packId == -1) {
    return PackKey();
  }

  sqlite3_stmt* stmt;

  std::string sqlQuery = "INSERT INTO pack_key (pack, key, uses) VALUES (?, ?, ?)";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, packId, key, uses)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  sqlite3_finalize(stmt);

  return PackKey(key, uses);
}

bool DatabaseManager::usePackKey(const std::string& key, const std::string& username) {
  if (!_userExists(username) || !_packKeyExists(key)) {
    return false;
  }

  _addPackAccount(username, _keyToPack(key));
  _decrementUses(key);
  removePackKey();
  return true;
}

void DatabaseManager::removePackKey(const std::string& key) {
  sqlite3_stmt* stmt;
  std::string sqlQuery;

  if (!key.empty()) {
    sqlQuery = "DELETE FROM pack_key WHERE key = ?";
  } else {
    sqlQuery = "DELETE FROM pack_key WHERE key = ? AND uses = 0";
  }

  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, key)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

LevelInfo DatabaseManager::getLevelInfo(int id) const {
  std::string name;
  std::string creator;
  int date;
  int rate;

  {
    sqlite3_stmt* stmt;

    std::string sqlQuery = "SELECT name, creator, date FROM levels WHERE id = ?";
    if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
      throw Error(sqlite3_errmsg(_db));
    }

    if (!_bindData(stmt, id)) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
      sqlite3_finalize(stmt);
      return LevelInfo();
    } else if (rc != SQLITE_ROW) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    if (name.empty()) {
      name = std::to_string(id);
    }
    creator = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    date = sqlite3_column_int(stmt, 2);
    sqlite3_finalize(stmt);
  }

  {
    sqlite3_stmt* stmt;

    std::string sqlQuery = "SELECT AVG(rate) FROM level_rating WHERE level = ?";
    if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
      throw Error(sqlite3_errmsg(_db));
    }

    if (!_bindData(stmt, id)) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
      sqlite3_finalize(stmt);
      return LevelInfo();
    } else if (rc != SQLITE_ROW) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    rate = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
  }

  return LevelInfo(id, creator, name, rate, date);
}

std::vector<LevelInfo>& DatabaseManager::getLevels(std::vector<LevelInfo>& dest, int nbEntries, int offset, const std::string& username) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT id, strftime('%s', date), name, creator FROM levels WHERE ";

  if (username.empty()) {
    sqlQuery += "creator <> 'tijl'";
  } else {
    sqlQuery += "creator = ?";
  }

  sqlQuery += " ORDER BY id";

  if (nbEntries != -1) {
    sqlQuery += " DESC LIMIT ? OFFSET ?";
  }

  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (username.empty()) {
    if (nbEntries != -1) {
      if (!_bindData(stmt, nbEntries, offset)) {
        sqlite3_finalize(stmt);
        throw Error(sqlite3_errmsg(_db));
      }
    }
  } else {
    if (nbEntries != -1) {
      if (!_bindData(stmt, username, nbEntries, offset)) {
        sqlite3_finalize(stmt);
        throw Error(sqlite3_errmsg(_db));
      }
    } else {
      if (!_bindData(stmt, username)) {
        sqlite3_finalize(stmt);
        throw Error(sqlite3_errmsg(_db));
      }
    }
  }

  int rc;
  while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    int levelID = sqlite3_column_int(stmt, 0);
    int date = sqlite3_column_int(stmt, 1);
    std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    if (name.empty()) {
      name = std::to_string(levelID);
    }
    std::string creator = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

    sqlite3_stmt* stmt2;
    std::string sqlQuery = "SELECT AVG(rate) FROM level_rating WHERE level = ?";
    if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt2, NULL) != SQLITE_OK) {
      throw Error(sqlite3_errmsg(_db));
    }

    if (!_bindData(stmt2, levelID)) {
      sqlite3_finalize(stmt2);
      throw Error(sqlite3_errmsg(_db));
    }

    int rc2 = sqlite3_step(stmt2);
    if (rc != SQLITE_ROW) {
      sqlite3_finalize(stmt2);
      throw Error(sqlite3_errmsg(_db));
    }

    int rate = sqlite3_column_int(stmt2, 0);
    sqlite3_finalize(stmt2);

    dest.push_back({levelID, creator, name, rate, date});
  }

  sqlite3_finalize(stmt);
  return dest;
}

std::map<unsigned, std::vector<EntityInfo>>& DatabaseManager::populateLevel(std::map<unsigned, std::vector<EntityInfo>>& level, int levelID) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT progress, entity, xPos, yPos, xSize, ySize, xVelocity, yVelocity FROM level_entities WHERE level = ? ORDER BY progress";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, levelID)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc;
  while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    unsigned progress = unsigned(sqlite3_column_int(stmt, 0));
    EntityInfo entity(unsigned(sqlite3_column_int(stmt, 1)), {double(sqlite3_column_int(stmt, 2)), double(sqlite3_column_int(stmt, 3)), sqlite3_column_int(stmt, 4), sqlite3_column_int(stmt, 5), double(sqlite3_column_int(stmt, 6)), double(sqlite3_column_int(stmt, 7))});

    if (level.find(progress) == level.end()) {
      std::vector<EntityInfo> vect;
      vect.push_back(entity);
      level.insert({progress, vect});
    } else {
      level.at(progress).push_back(entity);
    }
  }

  sqlite3_finalize(stmt);
  return level;
}

std::vector<EntityInfo>& DatabaseManager::populateLevel(std::vector<EntityInfo>& level, int id, unsigned progress) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "SELECT entity, xPos, yPos, xSize, ySize, xVelocity, yVelocity FROM level_entities WHERE level = ? AND progress = ?";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, id, int(progress))) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  int rc;
  while ((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    level.push_back({unsigned(sqlite3_column_int(stmt, 0)), {double(sqlite3_column_int(stmt, 1)), double(sqlite3_column_int(stmt, 2)), sqlite3_column_int(stmt, 3), sqlite3_column_int(stmt, 4), double(sqlite3_column_int(stmt, 5)), double(sqlite3_column_int(stmt, 6))}});
  }

  sqlite3_finalize(stmt);
  return level;
}

void DatabaseManager::addLevelEntity(int levelId, unsigned progress, const EntityInfo& entity) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "INSERT INTO level_entities (level, progress, entity, xPos, yPos, xSize, ySize, xVelocity, yVelocity) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, levelId, int(progress), int(entity.fullType()), int(entity.physicsBox().xPos), int(entity.physicsBox().yPos), int(entity.physicsBox().xSize), int(entity.physicsBox().ySize), int(entity.physicsBox().xVelocity), int(entity.physicsBox().yVelocity))) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  sqlite3_finalize(stmt);
}

void DatabaseManager::removeLevelEntity(int levelId, unsigned progress, const EntityInfo& entity) {
  sqlite3_stmt* stmt;

  std::string sqlQuery =
      "DELETE FROM level_entities \
                          WHERE id IN ( \
                            SELECT id \
                            FROM level_entities \
                            WHERE level = ? \
                              AND progress = ? \
                              AND entity = ? \
                              AND xPos = ? \
                              AND yPos = ? \
                              AND xSize = ? \
                              AND ySize = ? \
                              AND xVelocity = ? \
                              AND yVelocity = ? \
                            LIMIT 1 \
                          )";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, levelId, int(progress), int(entity.fullType()), int(entity.physicsBox().xPos), int(entity.physicsBox().yPos), int(entity.physicsBox().xSize), int(entity.physicsBox().ySize), int(entity.physicsBox().xVelocity), int(entity.physicsBox().yVelocity))) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  sqlite3_finalize(stmt);
}

int DatabaseManager::addLevel(const std::string& username, const std::string& levelName) {
  {
    sqlite3_stmt* stmt;

    std::string sqlQuery = "INSERT INTO levels (name, creator) VALUES (?, ?)";
    if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
      throw Error(sqlite3_errmsg(_db));
    }

    if (!_bindData(stmt, levelName, username)) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    sqlite3_finalize(stmt);
  }

  {
    sqlite3_stmt* stmt;

    std::string sqlQuery = "SELECT last_insert_rowid()";
    if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
      throw Error(sqlite3_errmsg(_db));
    }

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
      return -1;
    } else if (rc != SQLITE_ROW) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    int id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return id;
  }
}

void DatabaseManager::removeLevel(int id) {
  {
    sqlite3_stmt* stmt;

    std::string sqlQuery = "DELETE FROM level_entities WHERE id = ?";
    if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
      throw Error(sqlite3_errmsg(_db));
    }

    if (!_bindData(stmt, id)) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    sqlite3_finalize(stmt);
  }

  {
    sqlite3_stmt* stmt;

    std::string sqlQuery = "DELETE FROM levels WHERE id = ?";
    if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
      throw Error(sqlite3_errmsg(_db));
    }

    if (!_bindData(stmt, id)) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
      sqlite3_finalize(stmt);
      throw Error(sqlite3_errmsg(_db));
    }

    sqlite3_finalize(stmt);
  }
}

void DatabaseManager::setRate(int levelId, const std::string& username, int rate) {
  sqlite3_stmt* stmt;

  std::string sqlQuery = "REPLACE INTO level_rating (level, user, rate) VALUES (?, ?, ?)";
  if (sqlite3_prepare_v2(_db, sqlQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
    throw Error(sqlite3_errmsg(_db));
  }

  if (!_bindData(stmt, levelId, username, rate)) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    sqlite3_finalize(stmt);
    throw Error(sqlite3_errmsg(_db));
  }

  sqlite3_finalize(stmt);
}
