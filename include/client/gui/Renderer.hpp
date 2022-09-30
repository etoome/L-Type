#pragma once

#include <SDL2/SDL.h>

#include <array>
#include <forward_list>
#include <map>
#include <string>
#include <vector>

#include "client/gui/utils.hpp"

constexpr std::size_t NB_BACKGROUNDS = 5;

class Renderer {
 private:
  using SDL_Textures = std::map<std::string, SDL_Texture*>;
  using SDL_Fonts = std::map<std::string, TTF_Font*>;

  bool _backgroundScrolling = false;
  std::array<double, NB_BACKGROUNDS> _yBackgrounds = {0};

  SDL_Renderer* _renderer;
  SDL_Textures _textures = {};
  std::vector<Texture> _toRender = {};
  std::forward_list<Texture> _toRenderPermanent = {};  // Can only be removed explicitely

  SDL_Fonts _fonts = {};  // String keys are defined by _formatFontName()
  std::map<std::string, SDL_Color> _colors = {};

  std::string _formatFontName(const std::string& fontFamily, int fontSize);

  SDL_Texture* _getTexture(const std::string& textureName);

  void _renderTexture(const Texture&);
  void _renderBackground(std::size_t nBackground);

 public:
  Renderer(SDL_Window*);
  ~Renderer();
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;

  int width() const;
  int height() const;
  void getImageDimensions(const std::string& filePath, int& width, int& height);

  /* Return the image format width/height.
   */
  double getImageFormat(const std::string& filePath);

  void toggleBackgroundScrolling();
  void setCursor(SDL_Cursor*) const;

  void loadTexture(const std::string& filePath);
  void loadFont(const Font& fontInfo);
  void loadColor(const std::string& colorName, const SDL_Color& color);

  const SDL_Color& color(std::string colorName) const;
  SDL_Texture* createTextureFromText(const std::string& text, const Font& fontInfo, int maxWith = -1);

  void clear();

  /* Add a texture to the rendering list.
   * Warning: this texture will not be free after clearing.
   */
  void addTexture(const Texture&, bool permanent = false);
  void clearPermanentTextures();
  void addImage(const std::string& textureName, int xPos, int yPos, int width = 0, int height = 0, bool permanent = false);
  void addImage(const std::string& textureName, const SDL_Rect&, bool permanent = false);

  void renderSplashScreen();
  void render();
};
