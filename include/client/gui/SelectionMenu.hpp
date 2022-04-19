#pragma once

#include <vector>

#include "client/gui/Renderer.hpp"
#include "client/gui/Menu.hpp"
#include "client/gui/utils.hpp"

class SelectionMenu : public Menu {
 private:
  Texture* _titleTexture = nullptr;
  std::vector<Texture> _optionTextures = {};
  std::vector<Texture> _hOptionTextures = {};

  int _selectedOption;

  virtual void _loadChanges() override;
  SDL_Rect _getBorderGeometry(std::size_t nOption) const;

  bool _mouseIsOver();

  void _incCursor();
  void _decCursor();

 public:
  SelectionMenu(Renderer*, const std::string& title, int defaultOption, const std::vector<std::string>& options);
  ~SelectionMenu();
  SelectionMenu(const SelectionMenu&) = delete;
  SelectionMenu& operator=(const SelectionMenu&) = delete;

  std::size_t nOptions() const;
  int chooseOption();
};
