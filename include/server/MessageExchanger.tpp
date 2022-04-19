#pragma once

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <utility>

#include "Error.hpp"
#include "server/MessageExchanger.hpp"

template<typename Data, typename This>
void MessageExchanger::startListening(const std::string& channelName, CallbackFunction<Data, This> fct, This* objPtr) {
  if (_isChannelListening(channelName)) {
    throw Error("This pipe is already under listening");
  }

  openChannel(channelName);

  // Create a new thread to listen on this channel
  std::thread newThread(&MessageExchanger::_readMessages<Data, This>, this, channelName, fct, objPtr);
  _listeningThreads.insert({channelName, std::move(newThread)});
}

template<typename Data, typename This>
void MessageExchanger::_readMessages(const std::string& channelName, CallbackFunction<Data, This> fct, This* objPtr) {
  Data* data = static_cast<Data*>(malloc(sizeof(Data)));

  ssize_t n;
  // Read messages on pipe
  while ((n = readMessage(data, channelName)) != -1) {
    if (n != 0) {
      (objPtr->*fct)(*data);
    }
  }

  free(data);
}

template<typename Data>
ssize_t MessageExchanger::readMessage(Data* dest, const std::string& channelName, std::size_t nData) {
  // Open the channel if necessary
  openChannel(channelName);

  ssize_t n;
  if ((n = read(_fileDescriptors.at(channelName), dest, sizeof(Data) * nData)) == -1) {
    throw Error("Error while reading a message on the pipe " + channelName);
  }
  return n;
}

template<typename Data>
void MessageExchanger::writeMessage(const std::string& channelName, const Data& msg) {
  // Open the channel if necessary
  openChannel(channelName);

  if (write(_fileDescriptors.at(channelName), &msg, sizeof(Data)) == -1) {
    throw Error("Error while writing a message on the pipe " + channelName);
  }
}

template<typename Data>
void MessageExchanger::writeMessage(const std::string& channelName, const std::vector<Data>& data) {
  // Open the channel if necessary
  openChannel(channelName);

  if (write(_fileDescriptors.at(channelName), &data[0], sizeof(Data) * data.size()) == -1) {
    throw Error("Error while writing a message on the pipe " + channelName);
  }
}
