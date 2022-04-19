#include "server/MessageExchanger.hpp"

#include <sys/stat.h>
#include <sys/types.h>

#include "Error.hpp"
#include "GameSettings.hpp"

const std::string MessageExchanger::PIPE_DIR = "/tmp/l-type/";

MessageExchanger::~MessageExchanger() noexcept {
  ThreadsMap::iterator it = _listeningThreads.begin();
  while (it != _listeningThreads.end()) {
    stopListening(it->first);
  }

  for (const FileDescriptors::value_type& f: _fileDescriptors) {
    close(f.second);
  }
}

void MessageExchanger::init() const {
  // Create working directory
  if (mkdir(PIPE_DIR.c_str(), 0700) == -1) {
    if (errno != EEXIST) {
      throw FatalError("Error while creating the pipes directory");
    }
  }
}

void MessageExchanger::openChannel(const std::string& channelName) {
  if (_fileDescriptors.find(channelName) != _fileDescriptors.end()) return;

  if (mkfifo((PIPE_DIR + channelName).c_str(), 0600) == -1) {
    if (errno != EEXIST) {
      throw FatalError("Error while creating pipe " + channelName);
    }
  }

  int fd = open((PIPE_DIR + channelName).c_str(), O_RDWR);
  if (fd == -1) {
    throw FatalError("Error while opening pipe " + channelName);
  }

  _fileDescriptors.insert({channelName, fd});
}

void MessageExchanger::closeChannel(const std::string& channelName) {
  FileDescriptors::iterator fd = _fileDescriptors.find(channelName);
  if (fd != _fileDescriptors.end()) {
    close(fd->second);
    _fileDescriptors.erase(fd);
  }
}

void MessageExchanger::stopListening(const std::string& channelName) {
  // Cannot stop if not started
  if (!_isChannelListening(channelName)) {
    throw Error("This pipe is not under listening");
  }

  // Wait for the end of the thread and remove it from the mapping
  ThreadsMap::iterator it = _listeningThreads.find(channelName);
  it->second.join();
  _listeningThreads.erase(it);
}
