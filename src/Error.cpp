#include "Error.hpp"

Error::Error(const std::string& errorMessage) noexcept: _errorMessage(errorMessage) {}

Error::Error(const std::exception& error) noexcept: _errorMessage(error.what()) {}

const char* Error::what() const noexcept {
  return _errorMessage.c_str();
}

FatalError::FatalError(const std::string& errorMessage) noexcept: Error(errorMessage) {}

FatalError::FatalError(const std::exception& error) noexcept: Error(error) {}
