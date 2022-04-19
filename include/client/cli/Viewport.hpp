#pragma once

#include <ncurses.h>

#include <deque>
#include <map>
#include <string>
#include <vector>

#include "client/cli/assets/Sprite.hpp"
#include "client/cli/assets/UI/UI.hpp"
#include "constants.hpp"

/* A generic ncurses window.
 */
class Viewport {
 protected:
  constexpr static int MARGIN_X = 2;
  constexpr static int PADDING_X = 2 + 2 * MARGIN_X;
  constexpr static int PADDING_Y = 2;

  int _height = 0;
  int _width = 0;
  int _yOffset = 0;
  int _xOffset = 0;

  WINDOW* _outer = newwin(0, 0, 0, 0);
  WINDOW* _inner = newwin(0, 0, 0, 0);

  virtual int _maxInnerHeight() const noexcept;
  virtual int _maxInnerWidth() const noexcept;

  int _innerHeight() const noexcept;
  int _innerWidth() const noexcept;
  int _innerOffsetY() const noexcept;
  int _innerOffsetX() const noexcept;

 public:
  Viewport() = default;
  virtual ~Viewport() noexcept;
  Viewport(const Viewport&) = delete;
  Viewport& operator=(const Viewport&) = delete;

  /* Refresh the window.
   */
  virtual void drawWindow() noexcept;
  virtual void waitExit() noexcept;
};

/* A textual ncurses window.
 */
class TextViewport: public virtual Viewport {
 protected:
  std::string _textContent;

  int _maxInnerHeight() const noexcept override;
  int _maxInnerWidth() const noexcept override;

 public:
  TextViewport() = delete;
  ~TextViewport() noexcept override = default;
  TextViewport(const std::string&) noexcept;

  void drawWindow() noexcept override;
};

/* An input ncurses window.
 */
class InputViewport: public virtual Viewport {
 protected:
  int _yCursor = 0;
  int _xCursor = 0;

  void _incCursorX() noexcept;
  void _decCursorX() noexcept;

 public:
  InputViewport() = default;
  ~InputViewport() noexcept override = default;
};

/* A text input ncurses window.
 */
class TextInputViewport final: public TextViewport, public InputViewport {
 private:
  int _maxInnerHeight() const noexcept override;
  int _maxInnerWidth() const noexcept override;

 public:
  TextInputViewport(const std::string&) noexcept;
  ~TextInputViewport() noexcept override = default;

  /* Get input from stdin.
   * The return value is the input text.
   * Entered characters will be masked if the boolean parameter is set to true.
   * Can throw ExitSignal
   */
  std::string getText(size_t minLength, size_t maxLength, bool = false);
};
