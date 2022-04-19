#include "Message.hpp"

Handshake::Handshake(const SYN& syn) noexcept: AbstractMessage<SYN>(syn) {}

std::string Handshake::getUsername() const noexcept {
  return getData().username;
}

std::string Handshake::getPassword() const noexcept {
  return getData().password;
}

std::string Handshake::getChannel() const noexcept {
  return getData().listenedChannel.channelName;
}

bool Handshake::signingUp() const noexcept {
  return getData().signup;
}
