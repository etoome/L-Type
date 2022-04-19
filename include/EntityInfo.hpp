#pragma once

#include "PhysicsBox.hpp"

class EntityInfo {
 private:
  unsigned _typeID;
  PhysicsBox _physicsBox;

 public:
  EntityInfo(unsigned typeID, const PhysicsBox&);
  ~EntityInfo() = default;

  unsigned mainType() const;
  unsigned fullType() const;
  friend bool operator==(const EntityInfo&, const EntityInfo&);
  PhysicsBox physicsBox() const;
};


