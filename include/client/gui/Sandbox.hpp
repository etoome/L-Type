#pragma once

#include <SDL2/SDL.h>

#include <utils.hpp>
#include <vector>

#include "EntityInfo.hpp"
#include "SandboxEdition.hpp"
#include "client/gui/Activity.hpp"
#include "client/gui/Menu.hpp"
#include "client/gui/Renderer.hpp"
#include "client/gui/utils.hpp"

class Sandbox: public Activity, public Menu {
 private:
  SDL_Event _event = {};

  long int _lastSave = getTimestamp();
  unsigned _progress = 0;
  SDL_Texture* _progressTexture = nullptr;
  std::vector<Entity> _entitiesToDrag = {};
  std::vector<Entity> _entitiesToClick = {};
  std::size_t _selectedBoss = std::size_t(-1);

  EntityInfo _entityToEntityInfo(const Entity&, bool isBoss = false) const;
  Entity _entityInfoToEntity(const EntityInfo&) const;

  /* Returns the entity number on which the cursor is.
   * Returns -1 if the cursor is not on an entity.
   */
  std::size_t _cursorIsOnEntity(const std::vector<Entity>&) const;
  std::size_t _cursorIsOnBoss() const;
  bool _cursorIsOnSlider() const;

  void _getProgressSliderGeometry(SDL_Rect& barGeometry, SDL_Rect& cursorGeometry) const;
  int _progressBarWidth() const;
  virtual void _loadChanges() override;

  int _minTopX() const;
  int _maxTopX() const;

  void _drawTopLine();
  void _drawEntitiesContainer();
  void _drawProgressSlider();
  void _drawSavedIcon();

 public:
  Sandbox(Renderer*, const Mapping&);
  ~Sandbox() override = default;
  Sandbox(const Sandbox&) = delete;
  Sandbox& operator=(const Sandbox&) = delete;

  /* Handle events until a SandboxEdition is made.
   *
   * Will throw one of:
   *   - ExitSignal
   *   - SandboxEdition (addition)
   *   - SandboxEdition (deletion)
   *   - std::array<SandboxEdition, 2> ({deletion, addition})
   *   - unsigned (progress)
   */
  void handleEvents();
  virtual void refreshView() override;

  void updateEntities(const std::vector<EntityInfo> entities);
};
