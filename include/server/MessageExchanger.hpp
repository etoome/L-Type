#pragma once

#include <map>
#include <string>
#include <thread>
#include <vector>

/* A `MessageExchanger` allows processes to communicate by `Message`.
 * A `Message` in sent within a channel specified on writing or listening.
 * Channels are listened simultaneously.
 */
class MessageExchanger {
 private:
  static const std::string PIPE_DIR;

  // A callback function is supposed to be a member function
  template<typename Data, typename This>
  using CallbackFunction = void (This::*)(const Data&);

  using ThreadsMap = std::map<const std::string, std::thread>;
  ThreadsMap _listeningThreads = {};

  using FileDescriptors = std::map<const std::string, int>;
  FileDescriptors _fileDescriptors = {};

  /* Check whether a channel is under listening or not.
   */
  inline bool _isChannelListening(const std::string& channelName) const;

  /* Read a `Message` on the channel given as first parameter and apply a function on it.
   * The second parameter is a callback function to call with the message as parameter.
   * Note that the callback function is supposed to be a member function.
   */
  template<typename Data, typename This>
  void _readMessages(const std::string& channelName, CallbackFunction<Data, This> callback, This* thisArg);

 public:
  MessageExchanger() noexcept = default;
  ~MessageExchanger() noexcept;

  /* Initialize the message exchanger.
   */
  void init() const;

  /* Open a new communication channel.
   */
  void openChannel(const std::string& channelName);
  void closeChannel(const std::string& channelName);

  /* Start waiting a message on a specified channel.
   * The second parameter is a callback function to call with the message as parameter.
   * Note that the callback function is supposed to be a member function.
   */
  template<typename Data, typename This>
  void startListening(const std::string& channelName, CallbackFunction<Data, This> callback, This* thisArg);

  /* Stop waiting a message on a specified channel.
   */
  void stopListening(const std::string& channelName);

  /* Read a signle message on the given channelName and copy it in the given destination.
   * The return value can be:
   *  0 for EOF
   *  -1 for error
   *  positive value on success
   */
  template<typename Data>
  ssize_t readMessage(Data* dest, const std::string& channelName, std::size_t nData = 1);

  /* Write a Data on the channel given as first parameter.
   */
  template<typename Data>
  void writeMessage(const std::string& channelName, const Data& data);
  /* Write multiple Data on the channel given as first parameter.
   */
  template<typename Data>
  void writeMessage(const std::string& channelName, const std::vector<Data>& data);
};

inline bool MessageExchanger::_isChannelListening(const std::string& channelName) const {
  return _listeningThreads.find(channelName) != _listeningThreads.end();
  // return _listeningThreads.contains(channelName);
}

// Template definitions
#include "server/MessageExchanger.tpp"
