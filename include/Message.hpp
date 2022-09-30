#pragma once

#include <string>

#include "MessageData.hpp"
#include "Token.hpp"

template<typename T>
class AbstractMessage {
 private:
  const T _data;

 public:
  AbstractMessage() = delete;
  virtual ~AbstractMessage() noexcept = 0;
  AbstractMessage(const T&) noexcept;

  const T& getData() const noexcept;
};

/* Used to initiate communication.
 */
class Handshake: public AbstractMessage<SYN> {
 public:
  Handshake() = delete;
  ~Handshake() noexcept override = default;
  Handshake(const SYN& syn) noexcept;

  std::string getUsername() const noexcept;
  std::string getPassword() const noexcept;
  std::string getChannel() const noexcept;

  bool signingUp() const noexcept;
};

/* Used to communicate with the need to recognize each other.
 */
template<typename T>
class Message: public AbstractMessage<T> {
 private:
  const Token _token;

 public:
  Message() = delete;
  ~Message() noexcept override = default;
  Message(const Token& token, const T& data) noexcept;

  const Token& getToken() const noexcept;

  std::string getTokenSignature() const noexcept;
};

// Template definitions
#include "Message.tpp"
