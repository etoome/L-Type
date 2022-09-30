#pragma once

#include <map>

#include "client/cli/assets/Sprite.hpp"

class Assets final {
 private:
  static std::map<unsigned, Sprite*> _assets;

 public:
  Assets() = default;
  ~Assets() = default;

  static void buildAssets();
  static void destroyAssets();

  static Sprite* getSpriteById(unsigned id);
};
