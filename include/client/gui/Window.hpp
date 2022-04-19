#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "MessageData.hpp"
#include "SandboxEdition.hpp"
#include "client/gui/Activity.hpp"
#include "client/gui/Renderer.hpp"
#include "client/gui/assets/UI/Congratulations.hpp"
#include "client/gui/assets/UI/GameOver.hpp"
#include "client/gui/guiID.hpp"
#include "client/gui/utils.hpp"
#include "constants.hpp"

class Window {
 private:
  SDL_Window* _window = nullptr;
  Renderer* _renderer = nullptr;
  Activity* _currentActivity = nullptr;
  Font _currentFont = Fonts::DEFAULT_FONT;
  std::array<Mix_Chunk*, PACK_AUDIO_CHANNELS> _audios = {};

  void
  _initColors();

 public:
  Window();
  ~Window();
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  void _playSFX(int id);

  /* Print text directly in the root window.
   * Return the label identifier.
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
      const std::string& title,
      int defaultOption,
      const std::vector<Data>& vect,
      std::function<std::string(const Data&)> callback = [](const std::string& e) { return e; },
      const std::vector<std::string>& rawVect = {});

  int selectedPack();
  void setPack(int pack);

  /* -------------------- Game -------------------- */
  void initGame(const Activity::Mapping& userMapping);
  void refreshGame(const RefreshFrame&, const std::vector<EntityFrame>&);
  void win();
  void loose();
  void finishGame();

  /* Throws ExitSignal on exit.
   */
  template<typename This>
  void processGameInput(CallbackFunction<This, int> callback, This* objPtr);

  /* Draw a graphical UI for a given time then display a textual window with 'textAfter'.
   */
  void drawUI(const UI& ui, unsigned nSleepSeconds, const std::string& textAfter = "");

  /* -------------------- Sandbox -------------------- */
  template<typename This>
  void launchSandbox(
      void (This::*sendSandboxEdition)(const SandboxEdition&) const,
      std::vector<EntityInfo>& (This::*sendProgress)(std::vector<EntityInfo>&, unsigned) const,
      This* objPtr,
      const Activity::Mapping& userMapping);
};
// Template definitions
#include "client/gui/Window.tpp"
