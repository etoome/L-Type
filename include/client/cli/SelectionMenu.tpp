#pragma once

#include "client/cli/SelectionMenu.hpp"

template<typename... Args>
SelectionMenu::SelectionMenu(const std::string& title, int defaultOption, const Args&... args): InputViewport(), _title(title) {
  addOptions(args...);
  _yCursor = defaultOption;
}

template<typename... Args>
void SelectionMenu::addOptions(const std::string& firstOption, const Args&... options) {
  addOptions(firstOption);
  addOptions(options...);
}

template<>
inline void SelectionMenu::addOptions(const std::string& option) {
  _options.push_back(option);
}
