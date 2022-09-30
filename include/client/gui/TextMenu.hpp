#pragma once

#include <string>

#include "client/gui/Renderer.hpp"
#include "client/gui/utils.hpp"
#include "client/gui/Menu.hpp"

class TextMenu : public Menu {
 private:
  Texture* _texture = nullptr;
  bool _window;

  virtual void _loadChanges() override;

 public:
  TextMenu(Renderer* renderer, const std::string& text, const Font& = Fonts::DEFAULT_FONT, bool window = true);
  ~TextMenu();
  TextMenu(const TextMenu&) = delete;
  TextMenu& operator=(const TextMenu&) = delete;

  void waitExit();
};
