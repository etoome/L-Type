#pragma once

#include <exception>
#include <string>

#include "Error.hpp"

/* An `ErrorHandler` can handle errors whose messages are given by the calling program.
 * An handler can either append the error message in a log file or print it to stderr.
 * Passing a fatal error through the handler will cause the program to stop.
 */
class ErrorHandler {
 private:
  const std::string _logPath;
  bool _logToFile = true;

  /* Create the log directory
   * Set the handler to print errors on stderr if the operation failed.
   */
  inline void _createLogDirectory(const std::string&);

  inline void _logError(const std::exception&) const noexcept;
  inline void _printError(const std::exception&) const noexcept;

 public:
  ErrorHandler(const std::string&);
  ~ErrorHandler() noexcept = default;

  /* Log an error message in the log file.
   * If the log file was not created, the error message is printed to stderr.
   */
  void handleError(const std::exception&) const noexcept;
};
