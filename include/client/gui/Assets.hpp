#pragma once

#include <map>
#include <string>
#include <vector>

#include "client/gui/assets/Sprite.hpp"

class Assets final {
 public:
  using AudioMap = std::map<unsigned, std::vector<std::string>>;
  using FileMap = std::map<unsigned, std::string>;
  using AssetsMap = std::map<unsigned, std::tuple<int, int, std::vector<std::vector<std::vector<std::string>>>>>;

 private:
  static std::vector<std::string> _pack;
  static int _currentPack;

  static std::string _blankImage;
  static std::string _blankAudio;

  static AudioMap _audios;
  static FileMap _files;
  static AssetsMap _assets;

 public:
  Assets() = default;
  ~Assets() = default;

  static void setPack(int pack);
  static int getCurrentPack();

  static std::string getAudio(int id);
  static std::string getFile(int id);

  static std::vector<Sprite>& getEditableEntities(std::vector<Sprite>& dest);
  static Sprite getSprite(unsigned id, int state, int stateStep = 0, int variant = 0);
  static std::tuple<int, int> getSpriteSize(unsigned id);
};
