#pragma once

#include <cstring>
#include <string>

/* -------------------- Requests -------------------- */

struct Channel {
  char channelName[255];

  Channel(const std::string& _channelName) {
    strcpy(channelName, _channelName.c_str());
  }
};

struct SYN {
  char username[255];
  char password[255];
  Channel listenedChannel;
  bool signup;

  SYN(const std::string& _username, const std::string& _password, const std::string& _listenedChannel, bool _signup = false)
      : listenedChannel(_listenedChannel), signup(_signup) {
    strcpy(username, _username.c_str());
    strcpy(password, _password.c_str());
  }
};

struct LeaderboardRequest {
  char username[64] = "";  // For friendsOnly
  int nbEntries;
  int offset;

  LeaderboardRequest(const std::string& _username, int _nbEntries, int _offset): nbEntries(_nbEntries), offset(_offset) {
    strcpy(username, _username.c_str());
  }
};

struct PlayerInfoRequest {
  char username[64];

  PlayerInfoRequest(const std::string& _username) {
    strcpy(username, _username.c_str());
  }
};

struct FollowRequest {
  char username[64];
  bool add;  // add or remove

  FollowRequest(const std::string& _username, bool _add = true): add(_add) {
    strcpy(username, _username.c_str());
  }
};

struct PackKeyRequest {
  /**
   * 0 : use
   * 1 : list
   * 2 : add
   * 3 : remove
   */
  int command;
  char key[255];
  char pack[255];
  int uses;
  char username[255];

  PackKeyRequest(int c, const std::string& k = "", const std::string& p = "", int u = 1, const std::string& usr = ""): command(c), uses(u) {
    strcpy(key, k.c_str());
    strcpy(pack, p.c_str());
    strcpy(username, usr.c_str());
  }
};

struct LevelRequest {
  char user[255];
  int nbEntries;
  int offset;

  LevelRequest(const std::string& username, int _nbEntries, int _offset): nbEntries(_nbEntries), offset(_offset) {
    strcpy(user, username.c_str());
  }
};

struct LevelRate {
  int levelID;
  unsigned rating;  // [0, 5]
};

/* -------------------- Responses -------------------- */

struct ClientInfo {
  bool connected;
  bool admin;

  ClientInfo(bool c, bool a = false): connected(c), admin(a) {}
};

struct PlayerInfo {
  char username[64];
  int bestScore;
  int xp;
  bool isFollowed;
  bool isFollowingMe;

  PlayerInfo(const std::string& _username, int _bestScore, int _xp, bool _isFollowed = false, bool _isFollowingMe = false)
      : bestScore(_bestScore),
        xp(_xp),
        isFollowed(_isFollowed),
        isFollowingMe(_isFollowingMe) {
    strcpy(username, _username.c_str());
  }
};

struct Pack {
  int id;
  char name[255];
  bool owned;

  Pack(int i, const std::string& n = "", bool o = false): id(i), owned(o) {
    if (n.empty()) {
      strcpy(name, std::string("Pack " + i).c_str());
    } else {
      strcpy(name, n.c_str());
    }
  }
};

struct PackKey {
  char key[255];
  int uses;

  PackKey(const std::string& k = "", int u = 0): uses(u) {
    strcpy(key, k.c_str());
  }
};

struct RefreshFrame {
  int gameState;  // -1:lose; 0:running, 1:win
  long timestamp;
  unsigned int score[2];
  double hpPlayers[2];  // [0, NB_LIVES]
  unsigned progress;
  std::size_t nbEntities;  // number of EntityFrame to read
};

struct EntityFrame {
  unsigned id;  // binary chain type.subtype.subsubtype
  double xPos;
  double yPos;
  double hp;  // [0, 1]
  unsigned state;
  unsigned stateStep;
  unsigned variant;
};

struct LevelInfo {
  int id;
  char creator[255];
  char name[255];
  int rate;
  long int createdTimestamp;

  LevelInfo(int i = 0, const std::string& c = "", const std::string levelName = "", int r = 0, long int timestamp = 0)
      : id(i), rate(r), createdTimestamp(timestamp) {
    strcpy(creator, c.c_str());
    strcpy(name, levelName.c_str());
  }
};
