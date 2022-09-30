#pragma once

#include <exception>
#include <string>

/* A standard error wrapper.
 */
class Error: public std::exception {
 private:
  const std::string _errorMessage;

 public:
  Error(const std::string&) noexcept;
  Error(const std::exception&) noexcept;
  virtual ~Error() noexcept = default;

  const char* what() const noexcept override;
};

/* A fatal error may terminate the current process.
 */
class FatalError: public Error {
 public:
  FatalError(const std::string&) noexcept;
  FatalError(const std::exception&) noexcept;
};
