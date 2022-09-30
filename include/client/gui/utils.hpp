#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>

const std::string IMG_PATH = "static/img/";
const std::string FONTS_PATH = "static/fonts/";

/* A callback function is supposed to be a member function */
template<typename This, typename... Data>
using CallbackFunction = void (This::*)(Data...) const;

struct Font {
  std::string family;
  std::string color;
  int size;
};

struct SDL_Font {
  TTF_Font* family;
  SDL_Color color;
};

struct Texture {
  SDL_Texture* texture;
  SDL_Rect geometry;
};

int getKey(const SDL_Event&);

bool isMouseOver(const SDL_Rect& obj);

class Renderer;  // Defined in another header

int mapScaleX(Renderer*);
int mapScaleY(Renderer*);

class Fonts {
 private:
  static const Font _fonts[6];

 public:
  static const Font DEFAULT_FONT;
  static const Font MENU_FONT;
  static const Font MENU_H_FONT;
  static const Font TITLE_FONT;
  static const Font SUBTITLE_FONT;
  static const Font INPUT_FONT;

  static void initFonts(Renderer*);
};

class Cursors {
 public:
  static SDL_Cursor* ARROW;
  static SDL_Cursor* IBEAM;
  static SDL_Cursor* WAIT;
  static SDL_Cursor* CROSSHAIR;
  static SDL_Cursor* WAITARROW;
  static SDL_Cursor* SIZENWSE;
  static SDL_Cursor* SIZENESW;
  static SDL_Cursor* SIZEWE;
  static SDL_Cursor* SIZENS;
  static SDL_Cursor* SIZEALL;
  static SDL_Cursor* NO;
  static SDL_Cursor* HAND;

  static void initCursors();
  static void destroyCursors();
};
