#include "client/gui/Menu.hpp"

Menu::Menu(Renderer* renderer): _renderer(renderer) {}

void Menu::flushInputs() {
  while (SDL_PollEvent(&_event)) {}
}

void Menu::refreshView() {
  _renderer->clear();
  _loadChanges();
  _renderer->render();
}
