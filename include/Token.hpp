#pragma once

#include <string>

#include "constants.hpp"

class Token {
 private:
  char _username[255];
  char _guestUsername[255];
  char _activityID[255];
  char _timestamp[255];
  char _sig[255];

 public:
  Token() noexcept = default;
  ~Token() noexcept = default;
  Token(
      const std::string& username,
      const std::string& gameID,
      const std::string& timestamp,
      const std::string& signature,
      const std::string& guestUsername = "") noexcept;

  std::string getUsername() const noexcept;
  std::string getActivityID() const noexcept;
  std::string getTimestamp() const noexcept;
  std::string getSignature() const noexcept;
  std::string getGuestUsername() const noexcept;

  bool isEmpty() const noexcept;
};
