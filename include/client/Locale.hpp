#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>

class Locale {
 private:
  static std::map<std::string, unsigned> _langs;
  static std::array<std::map<std::string, std::vector<std::string>>, 3> _translations;

  static unsigned _lang;

 public:
  Locale() = delete;

  static unsigned getLanguageNumber() noexcept;
  static void setLanguage(const std::string& languageISO6391) noexcept;

  static const std::string& get(const std::string& englishText, std::size_t category = 0) noexcept;

  static constexpr std::size_t OTHER = 0;
  static constexpr std::size_t CONGRATULATIONS = 1;
  static constexpr std::size_t GAME_OVER = 2;

  static const std::string& getRandomSentence(std::size_t category) noexcept;
  static std::string ratingSentence(int rate);
};
