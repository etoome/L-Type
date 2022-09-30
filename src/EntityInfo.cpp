#include "EntityInfo.hpp"

EntityInfo::EntityInfo(unsigned typeID, const PhysicsBox& physBox)
    : _typeID(typeID), _physicsBox(physBox) {}

unsigned EntityInfo::mainType() const {
  return _typeID >> 4;
}

unsigned EntityInfo::fullType() const {
  return _typeID;
}

PhysicsBox EntityInfo::physicsBox() const {
  return _physicsBox;
}

bool operator==(const EntityInfo& entity1, const EntityInfo& entity2){
  if ((entity1.fullType() != entity2.fullType()) || (entity1._physicsBox.xPos != entity2._physicsBox.xPos) || (entity1._physicsBox.yPos != entity2._physicsBox.yPos))
  {
    return false;
  }
  return true;



}
