#pragma once

#include "client/gui/Renderer.hpp"
#include "client/gui/Menu.hpp"

class InputMenu : public Menu {
 private:

  Texture* _textTexture = nullptr;
  Texture* _inputTexture = nullptr;

  void _updateInputTexture(const std::string& inputTexture);
  void _clearInputTexture();
  virtual void _loadChanges() override;

 public:
  InputMenu(Renderer*, const std::string& text) noexcept;
  ~InputMenu();
  InputMenu(const InputMenu&) = delete;
  InputMenu& operator=(const InputMenu&) = delete;

  std::string enterText(std::size_t minLength, std::size_t maxLength, bool hidden = false);
};
