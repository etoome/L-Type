#include "utils.hpp"

#include <ctime>
#include <random>

std::string getStrTime(const std::string& timeFormat) noexcept {
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);

  char date[64];
  strftime(date, sizeof(date), timeFormat.c_str(), &tm);

  return date;
}

long int getTimestamp() noexcept {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  long int us = t.tv_sec * 1000000 + t.tv_nsec / 1000;
  return us;
}

std::string getStrTimestamp() noexcept {
  long int us = getTimestamp();
  char buffer[255];
  sprintf(buffer, "%ld", us);
  return std::string(buffer);
}

double genRandomDouble(double min, double max) noexcept {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(min, max);
  return dis(gen);
}

int genRandomInt(int min, int max) noexcept {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(min, max);
  return dis(gen);
}
