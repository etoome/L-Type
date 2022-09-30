#pragma once

#include "EntityInfo.hpp"

struct SandboxEdition {
  unsigned progress;  // %progress * 100
  EntityInfo entityInfo;
  bool add;  // add (true) remove (false)
};
