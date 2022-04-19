#include "client/gui/utils.hpp"

#include "Error.hpp"
#include "client/gui/Renderer.hpp"
#include "constants.hpp"

int getKey(const SDL_Event& event) {
  if (event.type == SDL_KEYDOWN) {
    return event.key.keysym.sym;
  } else if (event.type == SDL_TEXTINPUT) {
    if (event.text.text[1] == '\0') {
      return event.text.text[0];
    }
  }

  return INVALID_KEY;
}

int mapScaleX(Renderer* renderer) {
  return int(renderer->width() / MAP_WIDTH);
}

int mapScaleY(Renderer* renderer) {
  return int(renderer->height() / MAP_HEIGHT);
}

bool isMouseOver(const SDL_Rect& obj) {
  int xPos, yPos;
  SDL_GetMouseState(&xPos, &yPos);

  return (
      xPos >= obj.x &&
      xPos <= obj.x + obj.w &&
      yPos >= obj.y &&
      yPos <= obj.y + obj.h);
}

const Font Fonts::DEFAULT_FONT = {
    "OpenSans-Regular",
    "white",
    16,
};

const Font Fonts::MENU_FONT = {
    "SpaceRanger",
    "white",
    22,
};

const Font Fonts::MENU_H_FONT = {
    "SpaceRanger",
    "red",
    22,
};

const Font Fonts::TITLE_FONT = {
    "metallord",
    "white",
    75,
};

const Font Fonts::SUBTITLE_FONT = {
    "SpaceRanger",
    "white",
    25,
};

const Font Fonts::INPUT_FONT = {
    "vppixel",
    "white",
    18,
};

const Font Fonts::_fonts[] = {
    DEFAULT_FONT,
    MENU_FONT,
    MENU_H_FONT,
    TITLE_FONT,
    SUBTITLE_FONT,
    INPUT_FONT,
};

void Fonts::initFonts(Renderer* renderer) {
  for (const Font& font: _fonts) {
    renderer->loadFont(font);
  }
}

SDL_Cursor* Cursors::ARROW = nullptr;
SDL_Cursor* Cursors::IBEAM = nullptr;
SDL_Cursor* Cursors::WAIT = nullptr;
SDL_Cursor* Cursors::CROSSHAIR = nullptr;
SDL_Cursor* Cursors::WAITARROW = nullptr;
SDL_Cursor* Cursors::SIZENWSE = nullptr;
SDL_Cursor* Cursors::SIZENESW = nullptr;
SDL_Cursor* Cursors::SIZEWE = nullptr;
SDL_Cursor* Cursors::SIZENS = nullptr;
SDL_Cursor* Cursors::SIZEALL = nullptr;
SDL_Cursor* Cursors::NO = nullptr;
SDL_Cursor* Cursors::HAND = nullptr;

void Cursors::initCursors() {
  ARROW = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  IBEAM = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
  WAIT = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
  CROSSHAIR = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
  WAITARROW = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
  SIZENWSE = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
  SIZENESW = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
  SIZEWE = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
  SIZENS = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
  SIZEALL = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
  NO = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
  HAND = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
}

void Cursors::destroyCursors() {
  SDL_FreeCursor(ARROW);
  SDL_FreeCursor(IBEAM);
  SDL_FreeCursor(WAIT);
  SDL_FreeCursor(CROSSHAIR);
  SDL_FreeCursor(WAITARROW);
  SDL_FreeCursor(SIZENWSE);
  SDL_FreeCursor(SIZENESW);
  SDL_FreeCursor(SIZEWE);
  SDL_FreeCursor(SIZENS);
  SDL_FreeCursor(SIZEALL);
  SDL_FreeCursor(NO);
  SDL_FreeCursor(HAND);
}
