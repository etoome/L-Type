#include "server/Activity.hpp"

bool Activity::stopped() const {
  return _stopped;
}

void Activity::stop() {
  _stopped = true;
}
