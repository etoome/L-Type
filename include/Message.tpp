#pragma once

#include "Message.hpp"

template<typename T>
AbstractMessage<T>::~AbstractMessage() noexcept {}

template<typename T>
AbstractMessage<T>::AbstractMessage(const T& data) noexcept: _data(data) {}

template<typename T>
const T& AbstractMessage<T>::getData() const noexcept {
  return _data;
}

template<typename T>
Message<T>::Message(const Token& token, const T& data) noexcept: AbstractMessage<T>(data), _token(token) {}

template<typename T>
const Token& Message<T>::getToken() const noexcept {
  return _token;
}

template<typename T>
std::string Message<T>::getTokenSignature() const noexcept {
  return _token.getSignature();
}
