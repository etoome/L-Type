#include "ErrorHandler.hpp"

#include <sys/stat.h>

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "utils.hpp"

const std::string TIME_FILE_FORMAT = "%Y%m%d%H%M%S";
const std::string TIME_LOG_FORMAT = "%Y-%m-%d %H:%M:%S";

/* Build the full log path from the log directory
 */
inline std::string getLogPath(const std::string& logDir) noexcept {
  return logDir + getStrTime(TIME_FILE_FORMAT) + ".log";
}

ErrorHandler::ErrorHandler(const std::string& logDir): _logPath(getLogPath(logDir)) {
  _createLogDirectory(logDir);
}

inline void ErrorHandler::_createLogDirectory(const std::string& logDir) {
  if (mkdir(logDir.c_str(), 0700) == -1) {
    if (errno != EEXIST) {
      _logToFile = false;
      handleError(Error("Error while creating the log directory"));
    }
  };
}

inline void ErrorHandler::_logError(const std::exception& error) const noexcept {
  // Append the error message at the end of the log file
  std::ofstream logFile(_logPath, std::ios::app);
  logFile << "[" << getStrTime(TIME_LOG_FORMAT) << "] " << error.what() << std::endl;
  logFile.close();
}

inline void ErrorHandler::_printError(const std::exception& error) const noexcept {
  std::cerr << error.what() << std::endl;
}

void ErrorHandler::handleError(const std::exception& error) const noexcept {
  if (_logToFile) {
    _logError(error);
  } else {
    _printError(error);
  }

  // A fatal error will cause the program to stop.
  if (dynamic_cast<const FatalError*>(&error)) {
    std::exit(1);
  }
}
