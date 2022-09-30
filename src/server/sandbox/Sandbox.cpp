#include "server/sandbox/Sandbox.hpp"

#include <unistd.h>

#include <map>
#include <server/DatabaseManager.hpp>
#include <string>
#include <vector>

Sandbox::Sandbox(int id): Activity(), _levelId(id) {}

int Sandbox::getId() const {
  return _levelId;
}

void Sandbox::addEntity(unsigned progress, const EntityInfo& entityinfo) {
  if (_entities.find(progress) == _entities.end()) {
    _entities.insert({progress, {entityinfo}});
  } else {
    _entities[progress].push_back(entityinfo);
  }
}

void Sandbox::addEntities(unsigned progress, const std::vector<EntityInfo>& entityVector) {
  _entities.insert({progress, entityVector});
}

void Sandbox::addEntities(const std::map<unsigned, std::vector<EntityInfo>>& entityMap) {
  for (const auto& iter: entityMap) {
    addEntities(iter.first, iter.second);
  }
}

void Sandbox::delEntity(unsigned progress, const EntityInfo& entityinfo) {
  std::vector<EntityInfo> entitiesAtLocation;
  for (std::size_t e = 0; e != entitiesAtLocation.size(); ++e) {
    if (entitiesAtLocation[e] == entityinfo) {
      entitiesAtLocation.erase(entitiesAtLocation.begin() + std::size_t(e));
      break;
    }
  }
}
