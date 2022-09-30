#include "client/gui/Sandbox.hpp"

#include <string>

#include "Error.hpp"
#include "SandboxEdition.hpp"
#include "assetsID.hpp"
#include "client/KeyMapping.hpp"
#include "client/gui/Assets.hpp"
#include "client/gui/utils.hpp"
#include "constants.hpp"

const std::string SLIDER_BAR = IMG_PATH + "slider_bar.png";
const std::string SLIDER_CURSOR = IMG_PATH + "slider_cursor.png";
const std::string TOP_LINE = IMG_PATH + "hbar.png";
const std::string SAVED = IMG_PATH + "saved.png";
const std::string CONTAINER_BACKGROUND = IMG_PATH + "subwindow_background.png";
const std::string HIGHLIGHT_BOSS = IMG_PATH + "subwindow_background2.png";
constexpr double SAVED_ICON_TIMEOUT = 0.6;  // s
constexpr int SAVED_ICON_WIDTH = 40;        // px
constexpr int MARGIN = 15;                  // px

constexpr double TOP_BAR_WIDTH = 0.95;  // %screen
constexpr int TOP_BAR_HEIGHT = 5;       // px
constexpr int TOP_BAR_MARGIN = 30;      // px

constexpr double SLIDER_WIDTH = 0.75;  // %screen
constexpr int SLIDER_MARGIN = 10;      // px
constexpr double SLIDER_YPOS = 0.85;   // %screen

constexpr double CONTAINER_RELATIVE_MARGIN = 0.2;  // %screen
constexpr int CONTAINER_PADDING = 30;              // px
constexpr int ENTITY_MARGIN = 25;                  // px
constexpr int ENEMY_WIDTH = 3;                     // %screen
constexpr int BOSS_WIDTH = 8;                      // %screen

inline int containerMargin(Renderer* renderer, bool width) {  // true:width, false:height
  int size = (width) ? renderer->width() : renderer->height();
  return int(CONTAINER_RELATIVE_MARGIN * size);
}

Sandbox::Sandbox(Renderer* renderer, const Mapping& mapping): Activity(mapping), Menu(renderer) {
  int nexMaxEntityY = _renderer->height() * SLIDER_YPOS - containerMargin(_renderer, false);
  int nextEntityX = CONTAINER_PADDING + containerMargin(_renderer, true);

  std::vector<Sprite> sprites;
  for (const Sprite& entitySprite: Assets::getEditableEntities(sprites)) {
    switch (entitySprite.id >> ASSET_LEN_SID) {
      case ASSET_ENEMY_TYPE:
      case ASSET_OBSTACLE_TYPE: {
        double imageFormat = _renderer->getImageFormat(entitySprite.path);
        int entityHeight = int(double(ENEMY_WIDTH * mapScaleX(_renderer)) / imageFormat);
        SDL_Rect entityGeometry = {
            nextEntityX,
            nexMaxEntityY - entityHeight,
            ENEMY_WIDTH * mapScaleX(_renderer),
            entityHeight,
        };

        // Wrap to above row
        nextEntityX += entityGeometry.w + ENTITY_MARGIN;
        if (nextEntityX >= _renderer->width() - ENEMY_WIDTH * mapScaleX(_renderer) - CONTAINER_PADDING - containerMargin(_renderer, true)) {
          nextEntityX = CONTAINER_PADDING + containerMargin(_renderer, true);
          nexMaxEntityY -= entityGeometry.h + ENTITY_MARGIN;
        }

        _entitiesToDrag.push_back({entitySprite, entityGeometry.x, entityGeometry.y});
      } break;

      case ASSET_BOSS_TYPE: {
        double imageFormat = _renderer->getImageFormat(entitySprite.path);
        int entityHeight = int(double(BOSS_WIDTH * mapScaleX(_renderer)) / imageFormat);

        int yPos = 2 * MARGIN;
        if (_entitiesToClick.empty()) {
          yPos += _renderer->height() / 6;
        } else {
          double prevImageFormat = _renderer->getImageFormat(_entitiesToClick.back().sprite.path);
          int prevEntityHeight = int(double(BOSS_WIDTH * mapScaleX(_renderer)) / prevImageFormat);
          yPos += _entitiesToClick.back().yPos + prevEntityHeight + 2 * MARGIN;
        }

        SDL_Rect entityGeometry = {
            _renderer->width() - BOSS_WIDTH * mapScaleX(_renderer) - 2 * MARGIN,
            yPos,
            BOSS_WIDTH * mapScaleX(_renderer),
            entityHeight,
        };

        _entitiesToClick.push_back({entitySprite, entityGeometry.x, entityGeometry.y});
      } break;
    }
  }
}

void Sandbox::handleEvents() {
  flushInputs();

  std::size_t draggedEntity = -1;
  bool _hoverSlider = false;
  int xOffset;                                             // Cursor offset from the entity shape
  EntityInfo initialEntityInfo = {0, {0, 0, 0, 0, 0, 0}};  // Use on move

  bool quit = false;
  do {
    while (!quit && SDL_WaitEventTimeout(&_event, TICK / 1000)) {
      SDL_PollEvent(&_event);
      switch (_event.type) {
        case SDL_QUIT:
          throw FatalError("Game exit");
          break;

        case SDL_KEYDOWN:
          if (KeyMapping::matches(getKey(_event), GAME_KEY_ESC)) {
            throw ExitSignal();
          }
          break;

        case SDL_MOUSEBUTTONDOWN: {
          switch (_event.button.button) {
            case SDL_BUTTON_LEFT: {
              std::size_t e;
              // Left click on an entity in the container -> add this entity on top bar
              if ((e = _cursorIsOnEntity(_entitiesToDrag)) != -1) {
                _entities.push_back({
                    _entitiesToDrag[e].sprite,
                    int((1.0 - TOP_BAR_WIDTH) / 2 * _renderer->width()),
                    TOP_BAR_MARGIN + TOP_BAR_HEIGHT / 2,
                });

                _lastSave = getTimestamp();
                // Send addition to server
                throw SandboxEdition{_progress, _entityToEntityInfo(_entities.back()), true};
              }
              // Left click on boss
              else if ((e = _cursorIsOnBoss()) != -1) {
                _lastSave = getTimestamp();
                if (_selectedBoss == -1) {
                  // Send addition to server
                  throw SandboxEdition{BOSS_PROGRESS, _entityToEntityInfo(_entitiesToClick[_selectedBoss = e], true), true};
                } else {
                  // Send addition to server
                  throw std::array<SandboxEdition, 2>{
                      // Delection
                      SandboxEdition{BOSS_PROGRESS, _entityToEntityInfo(_entitiesToClick[_selectedBoss], true), false},
                      // Addition
                      SandboxEdition{BOSS_PROGRESS, _entityToEntityInfo(_entitiesToClick[_selectedBoss = e], true), true},
                  };
                }
              }
              // Left click on an entity -> drag it
              else if ((draggedEntity = _cursorIsOnEntity(_entities)) != -1) {
                initialEntityInfo = _entityToEntityInfo(_entities[draggedEntity]);
                xOffset = _event.button.x - _entities[draggedEntity].xPos;
              } else if (_cursorIsOnSlider()) {
                _hoverSlider = true;
              }
            } break;
            case SDL_BUTTON_RIGHT: {
              if (_hoverSlider || draggedEntity != -1) break;

              std::size_t e;
              // Right click on an entity -> remove it
              if ((e = _cursorIsOnEntity(_entities)) != -1) {
                EntityInfo entityToRemove = _entityToEntityInfo(_entities[e]);
                _entities.erase(_entities.begin() + e);

                _lastSave = getTimestamp();
                // Send deletion to server
                throw SandboxEdition{_progress, entityToRemove, false};
              }
            } break;
          }
        } break;

        case SDL_MOUSEBUTTONUP:
          if (draggedEntity != -1) {
            _lastSave = getTimestamp();
            // Send deletion + addition to server
            throw std::array<SandboxEdition, 2>{
                // Deletion
                SandboxEdition{_progress, initialEntityInfo, false},
                // Addition
                SandboxEdition{_progress, _entityToEntityInfo(_entities[draggedEntity]), true},
            };

            draggedEntity = -1;
          } else if (_hoverSlider) {
            _hoverSlider = false;
            throw _progress;
          }
          break;

        case SDL_MOUSEMOTION: {
          if (draggedEntity != -1) {
            if (_event.motion.x - xOffset >= _minTopX() && _event.motion.x - xOffset <= _maxTopX()) {
              _entities[draggedEntity].xPos = _event.motion.x - xOffset;
              refreshView();
            }
          } else if (_hoverSlider) {
            int xPos, yPos;
            SDL_GetMouseState(&xPos, &yPos);
            SDL_Rect barGeometry = {0, 0, 0, 0};
            SDL_Rect cursorGeometry = {0, 0, 0, 0};
            _getProgressSliderGeometry(barGeometry, cursorGeometry);

            int newProgress = int(double((xPos - barGeometry.x + cursorGeometry.w / 2)) / (double(_progressBarWidth()) / 100)) - 1;
            if (newProgress >= 0 && newProgress < 90) {
              _progress = newProgress;
              refreshView();
            }
          } else {
            if (_cursorIsOnEntity(_entitiesToDrag) != -1 || _cursorIsOnSlider() || _cursorIsOnEntity(_entities) != -1 || _cursorIsOnBoss() != -1) {
              SDL_SetCursor(Cursors::HAND);
            } else {
              SDL_SetCursor(Cursors::ARROW);
            }
          }
        } break;
      }
    }

    refreshView();
  } while (!quit);
}

Sandbox::Entity Sandbox::_entityInfoToEntity(const EntityInfo& entityInfo) const {
  return Entity{
      Assets::getSprite(entityInfo.fullType(), 0, 0, 0),
      int(entityInfo.physicsBox().xPos * mapScaleX(_renderer)),
      TOP_BAR_MARGIN + TOP_BAR_HEIGHT / 2,
  };
}

EntityInfo Sandbox::_entityToEntityInfo(const Entity& entity, bool isBoss) const {
  int typeID = entity.sprite.id;
  std::tuple<int, int> size = Assets::getSpriteSize(typeID);
  if (!isBoss) {
    return EntityInfo{
        unsigned(typeID),
        PhysicsBox{
            double(entity.xPos) / mapScaleX(_renderer),  // %map * 100
            0,
            std::get<0>(size),
            std::get<1>(size),
            0,
            0,
        },
    };
  } else {
    return EntityInfo{
        unsigned(typeID),
        PhysicsBox{
            double(100 - entity.sprite.width) / 2,
            0,
            std::get<0>(size),
            std::get<1>(size),
            0,
            0,
        },
    };
  }
}

std::size_t Sandbox::_cursorIsOnEntity(const std::vector<Entity>& entityVect) const {
  for (std::size_t e = entityVect.size() - 1; e != -1; --e) {
    int height = int(double(ENEMY_WIDTH * mapScaleX(_renderer)) / _renderer->getImageFormat(entityVect[e].sprite.path));

    if (isMouseOver({entityVect[e].xPos, entityVect[e].yPos, ENEMY_WIDTH * mapScaleX(_renderer), height})) {
      SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
      return e;
    }
  }

  return -1;
}

std::size_t Sandbox::_cursorIsOnBoss() const {
  for (std::size_t e = 0; e != _entitiesToClick.size(); ++e) {
    int height = int(double(BOSS_WIDTH * mapScaleX(_renderer)) / _renderer->getImageFormat(_entitiesToClick[e].sprite.path));

    if (isMouseOver({_entitiesToClick[e].xPos, _entitiesToClick[e].yPos, BOSS_WIDTH * mapScaleX(_renderer), height})) {
      SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
      return e;
    }
  }

  return -1;
}

bool Sandbox::_cursorIsOnSlider() const {
  SDL_Rect barGeometry = {0, 0, 0, 0};
  SDL_Rect cursorGeometry = {0, 0, 0, 0};
  _getProgressSliderGeometry(barGeometry, cursorGeometry);

  return isMouseOver(barGeometry) || isMouseOver(cursorGeometry);
}

void Sandbox::_getProgressSliderGeometry(SDL_Rect& barGeometry, SDL_Rect& cursorGeometry) const {
  int rendererWidth = _renderer->width();
  int rendererHeight = _renderer->height();

  barGeometry.w = _progressBarWidth() + 2 * SLIDER_MARGIN;
  barGeometry.h = int(barGeometry.w / _renderer->getImageFormat(SLIDER_BAR));
  barGeometry.x = int((rendererWidth - _progressBarWidth()) / 2 + SLIDER_MARGIN);
  barGeometry.y = int(rendererHeight * SLIDER_YPOS - barGeometry.h / 2);

  cursorGeometry.w = int(barGeometry.w / 50);
  cursorGeometry.h = int(cursorGeometry.w / _renderer->getImageFormat(SLIDER_CURSOR));
  cursorGeometry.x = barGeometry.x + SLIDER_MARGIN - cursorGeometry.w / 2 + (_progress * _progressBarWidth() / 100);
  cursorGeometry.y = int(rendererHeight * SLIDER_YPOS - cursorGeometry.h / 2);
}

int Sandbox::_progressBarWidth() const {
  return int(SLIDER_WIDTH * _renderer->width());
}

int Sandbox::_minTopX() const {
  return int((1.0 - TOP_BAR_WIDTH) / 2 * _renderer->width());
}

int Sandbox::_maxTopX() const {
  return int(TOP_BAR_WIDTH * _renderer->width() + (1.0 - TOP_BAR_WIDTH) / 2 * _renderer->width() - ENEMY_WIDTH * mapScaleX(_renderer));
}

void Sandbox::_drawTopLine() {
  SDL_Rect topBarGeometry = {
      _minTopX(),
      TOP_BAR_MARGIN,
      int(TOP_BAR_WIDTH * _renderer->width()),
      TOP_BAR_HEIGHT,
  };
  _renderer->addImage(TOP_LINE, topBarGeometry);
}

void Sandbox::_drawEntitiesContainer() {
  for (const Entity& entity: _entitiesToDrag) {
    int height = (entity.sprite.width * mapScaleX(_renderer)) / _renderer->getImageFormat(entity.sprite.path);
    _renderer->addImage(entity.sprite.path, {entity.xPos, entity.yPos, entity.sprite.width * mapScaleX(_renderer), height});
  }

  for (std::size_t e = 0; e != _entitiesToClick.size(); ++e) {
    Entity& entity = _entitiesToClick[e];
    int height = BOSS_WIDTH * mapScaleX(_renderer) / _renderer->getImageFormat(entity.sprite.path);

    if (e == _selectedBoss) {
      SDL_Rect boxGeometry = {
          entity.xPos - MARGIN / 2,
          entity.yPos - MARGIN / 2,
          BOSS_WIDTH * mapScaleX(_renderer) + MARGIN,
          height + MARGIN,
      };
      _renderer->addImage(HIGHLIGHT_BOSS, boxGeometry);
    }

    _renderer->addImage(entity.sprite.path, {entity.xPos, entity.yPos, BOSS_WIDTH * mapScaleX(_renderer), height});
  }
}

void Sandbox::_drawProgressSlider() {
  int rendererWidth = _renderer->width();
  int rendererHeight = _renderer->height();

  SDL_Rect barGeometry = {0, 0, 0, 0};
  SDL_Rect cursorGeometry = {0, 0, 0, 0};
  _getProgressSliderGeometry(barGeometry, cursorGeometry);

  SDL_Texture* _progressTexture = _renderer->createTextureFromText(std::to_string(_progress) + "%", Fonts::DEFAULT_FONT);
  SDL_Rect progressGeometry = {0, 0, 0, 0};
  SDL_QueryTexture(_progressTexture, NULL, NULL, &progressGeometry.w, &progressGeometry.h);
  progressGeometry.x = cursorGeometry.x - (progressGeometry.w - cursorGeometry.w) / 2;
  progressGeometry.y = cursorGeometry.y - progressGeometry.h - SLIDER_MARGIN;

  _renderer->addImage(SLIDER_BAR, barGeometry.x, barGeometry.y, barGeometry.w, barGeometry.h);
  _renderer->addImage(SLIDER_CURSOR, cursorGeometry.x, cursorGeometry.y, cursorGeometry.w, cursorGeometry.h);
  _renderer->addTexture({_progressTexture, progressGeometry});
}

void Sandbox::_drawSavedIcon() {
  int height = SAVED_ICON_WIDTH / _renderer->getImageFormat(SAVED);
  int xPos = _renderer->width() - SAVED_ICON_WIDTH - MARGIN;
  int yPos = _renderer->height() - height - MARGIN;
  _renderer->addImage(SAVED, {xPos, yPos, SAVED_ICON_WIDTH, height});
}

void Sandbox::_loadChanges() {
  _drawTopLine();
  for (const Entity& entity: _entities) {
    int height = (entity.sprite.width * mapScaleX(_renderer)) / _renderer->getImageFormat(entity.sprite.path);
    _renderer->addImage(entity.sprite.path, {entity.xPos, entity.yPos, entity.sprite.width * mapScaleX(_renderer), height});
  }

  _drawEntitiesContainer();
  _drawProgressSlider();

  if (getTimestamp() - _lastSave < SAVED_ICON_TIMEOUT * 1000000) {
    _drawSavedIcon();
  }
}

void Sandbox::refreshView() {
  Menu::refreshView();
  SDL_DestroyTexture(_progressTexture);
  _progressTexture = nullptr;
}

void Sandbox::updateEntities(const std::vector<EntityInfo> entities) {
  _entities.clear();
  for (const EntityInfo& entityInfo: entities) {
    if (entityInfo.fullType() >> ASSET_LEN_SID == ASSET_BOSS_TYPE) {
      _selectedBoss = entityInfo.fullType() % ASSET_LEN_SID;
    } else {
      _entities.push_back(_entityInfoToEntity(entityInfo));
    }
  }
}
