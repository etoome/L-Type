#pragma once

#include <SDL2/SDL.h>

#include "client/gui/Renderer.hpp"

class Menu {
 protected:
  Renderer* _renderer;
  SDL_Event _event = {};

  virtual void _loadChanges() = 0;

  void flushInputs();

 public:
  explicit Menu(Renderer* renderer);
  virtual ~Menu() = default;
  Menu(const Menu&) = delete;
  Menu& operator=(const Menu&) = delete;

  virtual void refreshView();
};
