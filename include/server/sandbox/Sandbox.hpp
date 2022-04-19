#pragma once

#include <map>
#include <string>
#include <vector>

#include "EntityInfo.hpp"
#include "server/Activity.hpp"

class Sandbox: public Activity {
 private:
  int _levelId;
  std::map<unsigned, std::vector<EntityInfo>> _entities = {};

 public:
  explicit Sandbox(int);
  ~Sandbox() override = default;

  int getId() const;

  void addEntity(unsigned progress, const EntityInfo& entityinfo);
  void addEntities(unsigned progress, const std::vector<EntityInfo>& entityVector);
  void addEntities(const std::map<unsigned, std::vector<EntityInfo>>& entityMap);
  void delEntity(unsigned progress, const EntityInfo& entityinfo);
};
