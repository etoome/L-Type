#pragma once

class Activity {
 protected:
  bool _stopped = false;

 public:
  Activity() = default;
  virtual ~Activity() = default;

  bool stopped() const;

  void stop();
};
