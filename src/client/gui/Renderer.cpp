#include "client/gui/Renderer.hpp"

#include <SDL2/SDL_image.h>

#include "Error.hpp"
#include "client/gui/Assets.hpp"
#include "client/gui/guiID.hpp"
#include "constants.hpp"

std::vector<std::string> SPLASH_SCREEN_IMGS = {
    "static/spash_screens/main.png",
};

std::array<int, NB_BACKGROUNDS> BACKGROUND_IMGS = {
    PACK_BACKGROUND_0_ID,
    PACK_BACKGROUND_1_ID,
    PACK_BACKGROUND_2_ID,
    PACK_BACKGROUND_3_ID,
    PACK_BACKGROUND_4_ID,
};

std::array<double, NB_BACKGROUNDS> BACKGROUND_SPEEDS = {
    // px/frame
    0.9,
    0.75,
    0.4,
    0.33,
    0.15,
};

Renderer::Renderer(SDL_Window* window): _renderer(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)) {
  if (!IMG_Init(IMG_INIT_PNG)) {
    char errMessage[255];
    sprintf(errMessage, "Unable to IMG_Init: %s", SDL_GetError());
    throw FatalError(errMessage);
  }
  if (TTF_Init() == -1) {
    char errMessage[255];
    sprintf(errMessage, "Unable to TTF_Init: %s", SDL_GetError());
    throw FatalError(errMessage);
  }
}

Renderer::~Renderer() {
  clear();

  for (const SDL_Fonts::value_type& font: _fonts) {
    TTF_CloseFont(font.second);
  }
  TTF_Quit();

  for (const SDL_Textures::value_type& texture: _textures) {
    SDL_DestroyTexture(texture.second);
  }
  IMG_Quit();

  SDL_DestroyRenderer(_renderer);
}

int Renderer::width() const {
  int rendererWidth;
  SDL_GetRendererOutputSize(_renderer, &rendererWidth, NULL);
  return rendererWidth;
}

int Renderer::height() const {
  int rendererHeight;
  SDL_GetRendererOutputSize(_renderer, NULL, &rendererHeight);
  return rendererHeight;
}

void Renderer::getImageDimensions(const std::string& filePath, int& width, int& height) {
  loadTexture(filePath);
  SDL_QueryTexture(_getTexture(filePath), NULL, NULL, &width, &height);
}

double Renderer::getImageFormat(const std::string& filePath) {
  int width, height;
  getImageDimensions(filePath, width, height);
  return double(width) / height;
}

void Renderer::toggleBackgroundScrolling() {
  _backgroundScrolling = !_backgroundScrolling;
}

void Renderer::setCursor(SDL_Cursor* cursor) const {
  SDL_SetCursor(cursor);
}

void Renderer::loadTexture(const std::string& filePath) {
  if (!(_textures.count(filePath) > 0)) {  // if not in map
    SDL_Texture* texture = IMG_LoadTexture(_renderer, filePath.c_str());

    if (texture == NULL) {
      char errMessage[255];
      sprintf(errMessage, "Unable to load texture %s", filePath.c_str());
      throw FatalError(errMessage);
    }

    _textures.insert({filePath, texture});
  }
}

void Renderer::loadFont(const Font& fontInfo) {
  TTF_Font* sdlFont = TTF_OpenFont((FONTS_PATH + fontInfo.family + ".ttf").c_str(), fontInfo.size);

  if (sdlFont == NULL) {
    char errMessage[255];
    sprintf(errMessage, "Unable to load font %s", (FONTS_PATH + fontInfo.family + ".ttf").c_str());
    throw FatalError(errMessage);
  }

  _fonts.insert({_formatFontName(fontInfo.family, fontInfo.size), sdlFont});
}

std::string Renderer::_formatFontName(const std::string& fontFamily, int fontSize) {
  return fontFamily + "." + std::to_string(fontSize);
}

SDL_Texture* Renderer::_getTexture(const std::string& textureName) {
  loadTexture(textureName);
  return _textures.at(textureName);
}

void Renderer::loadColor(const std::string& colorName, const SDL_Color& color) {
  _colors.insert({colorName, color});
}

void Renderer::_renderTexture(const Texture& texture) {
  SDL_Rect geometryBox = texture.geometry;

  // Resize the texture if necessary
  if (geometryBox.w == 0 && geometryBox.h == 0) {
    SDL_QueryTexture(texture.texture, NULL, NULL, &geometryBox.w, &geometryBox.h);
  } else if (geometryBox.w == 0 || geometryBox.h == 0) {
    int width;
    int height;
    SDL_QueryTexture(texture.texture, NULL, NULL, &width, &height);

    if (geometryBox.w == 0) {
      geometryBox.w = width * geometryBox.h / height;
    } else {
      geometryBox.h = height * geometryBox.w / width;
    }
  }

  SDL_RenderCopy(_renderer, texture.texture, NULL, &geometryBox);
}

const SDL_Color& Renderer::color(std::string colorName) const {
  return _colors.at(colorName);
}

SDL_Texture* Renderer::createTextureFromText(const std::string& text, const Font& fontInfo, int maxWidth) {
  if (maxWidth == -1) {
    maxWidth = width();
  }

  SDL_Surface* surface = TTF_RenderUTF8_Blended_Wrapped(_fonts.at(_formatFontName(fontInfo.family, fontInfo.size)), text.c_str(), _colors.at(fontInfo.color), maxWidth);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
  SDL_FreeSurface(surface);
  return texture;
}

void Renderer::clear() {
  SDL_RenderClear(_renderer);
  _toRender.clear();
}

void Renderer::addTexture(const Texture& texture, bool permanent) {
  if (permanent) {
    _toRenderPermanent.push_front(texture);
  } else {
    _toRender.push_back(texture);
  }
}

void Renderer::clearPermanentTextures() {
  _toRenderPermanent.clear();
}

void Renderer::addImage(const std::string& texturePath, int xPos, int yPos, int width, int height, bool permanent) {
  loadTexture(texturePath);
  addTexture({_getTexture(texturePath), {xPos, yPos, width, height}}, permanent);
}

void Renderer::addImage(const std::string& texturePath, const SDL_Rect& geometry, bool permanent) {
  loadTexture(texturePath);
  addTexture({_getTexture(texturePath), {geometry.x, geometry.y, geometry.w, geometry.h}}, permanent);
}

void Renderer::_renderBackground(std::size_t nBackground) {
  int backgroundWidth, backgroundHeight;
  SDL_QueryTexture(_getTexture(Assets::getFile(BACKGROUND_IMGS[nBackground])), NULL, NULL, &backgroundWidth, &backgroundHeight);

  SDL_Rect backgroundSrcGeometry1 = {0, 0, backgroundWidth, backgroundHeight};
  SDL_Rect backgroundSrcGeometry2 = {0, 0, backgroundWidth, backgroundHeight};

  int height = backgroundHeight * width() / backgroundWidth;

  SDL_Rect backgroundDstGeometry1 = {0, int(_yBackgrounds[nBackground]), width(), height};
  SDL_Rect backgroundDstGeometry2 = {0, int(_yBackgrounds[nBackground]) - height, width(), height};

  SDL_RenderCopy(_renderer, _getTexture(Assets::getFile(BACKGROUND_IMGS[nBackground])), &backgroundSrcGeometry1, &backgroundDstGeometry1);
  SDL_RenderCopy(_renderer, _getTexture(Assets::getFile(BACKGROUND_IMGS[nBackground])), &backgroundSrcGeometry2, &backgroundDstGeometry2);

  if (_backgroundScrolling) {
    _yBackgrounds[nBackground] += BACKGROUND_SPEEDS[nBackground];
    if (_yBackgrounds[nBackground] >= height) {
      _yBackgrounds[nBackground] = 0;
    }
  }
}

void Renderer::renderSplashScreen() {
  for (size_t ss = 0; ss != SPLASH_SCREEN_IMGS.size(); ++ss) {
    SDL_Rect splashScreenGeometry = {0, 0, 0, 0};
    splashScreenGeometry.w = width();
    splashScreenGeometry.h = splashScreenGeometry.w / getImageFormat(SPLASH_SCREEN_IMGS[ss]);
    splashScreenGeometry.x = (width() - splashScreenGeometry.w) / 2;
    splashScreenGeometry.y = (height() - splashScreenGeometry.h) / 2;

    SDL_RenderCopy(_renderer, _getTexture(SPLASH_SCREEN_IMGS[ss]), NULL, &splashScreenGeometry);
    SDL_RenderPresent(_renderer);

    SDL_Delay(SPLASH_SCREEN_DELAY);
  }
}

void Renderer::render() {
  for (std::size_t b = 0; b != NB_BACKGROUNDS; ++b) {
    _renderBackground(b);
  }

  for (const Texture& texture: _toRenderPermanent) {
    _renderTexture(texture);
  }
  for (const Texture& texture: _toRender) {
    _renderTexture(texture);
  }

  SDL_RenderPresent(_renderer);
}
