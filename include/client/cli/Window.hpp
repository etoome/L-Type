#pragma once

#include <functional>
#include <string>
#include <vector>

#include "MessageData.hpp"
#include "client/cli/Game.hpp"
#include "client/cli/Viewport.hpp"
#include "client/cli/assets/UI/Congratulations.hpp"
#include "client/cli/assets/UI/GameOver.hpp"
#include "constants.hpp"

class Window {
 private:
  /* A callback function is supposed to be a member function */
  template<typename Data, typename This>
  using CallbackFunction = void (This::*)(Data) const;

  static void _resizeHandler(int signal);

  Game* _currentGame = nullptr;

  Viewport* _currentWindow = nullptr;
  std::vector<WINDOW*> _labels = {};

  void _popup(int, int) noexcept;
  void _eraseWindow();

  /* -------------------- Inputs -------------------- */
  void _flushInputs();

 public:
  Window();
  ~Window();
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  /* Print text directly in the root window.
   */
  void drawLabel(const std::string& label, int yPos, int xPos);
  void clearLabels();

  /* Display a countdown and wait for the end.
   */
  void drawCountdown(unsigned i);

  /* Display a textual subwindow for a given time.
   *  -1 to wait for input before closing subwindow.
   */
  void drawTextMenu(const std::string& text, unsigned displayTime = unsigned(-1));

  /* Display an input subwindow and return the input.
   *  Throws ExitSignal on exit.
   */
  std::string drawInputMenu(const std::string& text, size_t minLength = 0, size_t maxLength = 64, bool hiddenText = false);

  /* Display a subwindow with the specified options after passing them in the callback.
   * The values of the last vector will not be passed through the callback.
   * The return value is the choosen option [0, ...]
   */
  template<typename Data>
  int drawSelectionMenu(
      const std::string& title, int defaultOption, const std::vector<Data>& vect, std::function<std::string(const Data&)> callback = [](const std::string& e) { return e; }, const std::vector<std::string>& rawVect = {});

  int selectedPack();
  void setPack(int pack);

  /* -------------------- Game -------------------- */
  void initGame(const Game::Mapping& userMapping);
  void refreshGame(const RefreshFrame&, const std::vector<EntityFrame>&);
  void win();
  void loose();
  void finishGame();

  /* Throws ExitSignal on exit.
   */
  template<typename This>
  void processGameInput(CallbackFunction<int, This> callback, This* objPtr);

  /* Draw a graphical UI for a given time then display a textual window with 'textAfter'.
   */
  void drawUI(const UI& ui, unsigned nSleepSeconds, const std::string& textAfter = "");
};

// Template definitions
#include "client/cli/Window.tpp"
