#pragma once

#include <string>
#include <vector>

#include "client/cli/Viewport.hpp"
#include "client/cli/assets/Sprite.hpp"

class SelectionMenu: public virtual InputViewport {
 private:
  int _maxInnerHeight() const noexcept override;
  int _maxInnerWidth() const noexcept override;

  void _highlight() noexcept;

 protected:
  std::string _title;
  std::vector<std::string> _options = {};

  int _highlighted = 0;

  virtual void _incCursorY() noexcept;
  virtual void _decCursorY() noexcept;

 public:
  SelectionMenu(const std::string& title, int defaultOption = 0);
  ~SelectionMenu() noexcept override = default;
  /* All options are passed to the constructor.
   */
  template<typename... Args>
  SelectionMenu(const std::string& title, int defaultOption, const Args&... options);

  template<typename... Args>
  void addOptions(const std::string& firstOption, const Args&... options);
  void addOptions() {}

  void drawWindow() noexcept override;

  /* Ask the user to choose between options.
   */
  int chooseOption();
};

// Template definitions
#include "client/cli/SelectionMenu.tpp"

class SelectionMenuPreview final: public SelectionMenu {
 private:
  std::vector<const Sprite*> _sprites = {};

  int _maxInnerWidth() const noexcept override;
  void _incCursorY() noexcept override;
  void _decCursorY() noexcept override;

 public:
  using Sprites = std::vector<std::pair<std::string, const Sprite*>>;

  SelectionMenuPreview(const std::string& title, const Sprites& sprites, int highlighted);

  void drawWindow() noexcept override;
};
