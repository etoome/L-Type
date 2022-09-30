#include "Token.hpp"

#include <cstring>

Token::Token(
    const std::string& username,
    const std::string& gameID,
    const std::string& timestamp,
    const std::string& sig,
    const std::string& guestUsername) noexcept {
  strcpy(_username, username.c_str());
  strcpy(_guestUsername, guestUsername.c_str());
  strcpy(_activityID, gameID.c_str());
  strcpy(_timestamp, timestamp.c_str());
  strcpy(_sig, sig.c_str());
}

std::string Token::getUsername() const noexcept {
  return _username;
}

std::string Token::getActivityID() const noexcept {
  return _activityID;
}

std::string Token::getTimestamp() const noexcept {
  return _timestamp;
}

std::string Token::getSignature() const noexcept {
  return _sig;
}

std::string Token::getGuestUsername() const noexcept {
  return _guestUsername;
}

bool Token::isEmpty() const noexcept {
  return _sig[0] == '\0';
}
