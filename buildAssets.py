#!/usr/bin/env python3

import os
import re

LEN_PREFIX = 4
LEN_SID = 4  # subtype

PREFIX = {"Player_": {"pre": 0, "current": 0},
          "Enemy_": {"pre": 1, "current": 0},
          "Boss_": {"pre": 2, "current": 0},
          "Henchman_": {"pre": 3, "current": 0},
          "Obstacle_": {"pre": 4, "current": 0},
          "Powerup_": {"pre": 5, "current": 0},
          "Bullet": {"pre": 6, "current": 0}}


STATES = {
    "idle": 0,
    "move": 1,
    "shoot": 2,
    "hurt": 3,
    "die": 4,
    "respawn": 5,
    "power_up": 6
}


ASSETS_DIR = "include/client/cli/assets"

ASSETS_ID_FILE = "include/assetsID.hpp"
GUI_ID_FILE = "include/client/gui/guiID.hpp"
MAP_FILE_CLI = "src/client/cli/Assets.cpp"
MAP_FILE_GUI = "src/client/gui/Assets.cpp"

PACK_DIR = "static/packs"
DEFAULT_PACK = "StorWors"

BLANK_IMAGE = "static/packs/blank.png"
BLANK_AUDIO = "static/packs/blank.ogg"


# 0001   0001
# type + subtype <-- id assets
#  ^
#  |
#  id server (Entity)

def genId(asset: str) -> int:

    prefix = None

    for p in PREFIX:
        if asset.startswith(p):
            prefix = PREFIX[p]
            break
    if prefix == None:
        return None

    sid = prefix["current"]
    prefix["current"] += 1

    return (prefix["pre"] << LEN_SID)+sid


def getAssetSize(path: str) -> [int, int]:
    height = 0
    width = 0

    with open(path) as f:
        for line in f.readlines():
            w = re.match(
                ".*_width\s*=\s*(\d+)\s*\n", line)
            if w != None:
                width = int(w.group(1))

            l = re.match(
                ".*_height\s*=\s*(\d+)\s*\n", line)
            if l != None:
                height = int(l.group(1))

    return [width, height]


class AssetsID:
    def __init__(self, file: str):
        self._file = file
        self._spritesID = []

    def add(self, key: str, value: int):
        self._spritesID.append((key, value))

    def write(self):
        with open(self._file, 'w') as f:
            f.write(f'#pragma once\n\n')
            f.write(
                f'constexpr int ASSET_LEN_PREFIX = {LEN_PREFIX};\nconstexpr int ASSET_LEN_SID = {LEN_SID};\n\n')
            for p in PREFIX:
                f.write(
                    f'constexpr int ASSET_{p.replace("_", "").upper()}_TYPE = {PREFIX[p]["pre"]};\n')
            f.write("\n")
            for (key, value) in self._spritesID:
                f.write(f'constexpr int ASSET_{key} = {value};\n')
            f.close()


class GuiID:
    def __init__(self, file: str):
        self._file = file
        self._spritesID = []
        self._audios = 0

    def add(self, key: str, value: int):
        self._spritesID.append((key, value))

    def addAudio(self, key: str, value: int):
        self._spritesID.append((key, value))
        self._audios += 1

    def write(self):
        with open(self._file, 'w') as f:
            f.write(f'#pragma once\n\n')
            f.write(f'constexpr int PACK_AUDIO_CHANNELS = {self._audios};\n\n')
            for (key, value) in self._spritesID:
                f.write(f'constexpr int PACK_{key} = {value};\n')
            f.close()


class CliAssetsMap:
    def __init__(self, file: str):
        self._file = file
        self._sprites = []

    def addSprite(self, sprite: str):
        self._sprites.append(sprite)

    def write(self):
        with open(self._file, 'w') as f:
            f.write(
                '''#include "client/cli/Assets.hpp"

#include "assetsID.hpp"
#include "client/cli/assets/Sprite.hpp"
''')
            for s in self._sprites:
                f.write(f'#include "client/cli/assets/{s}.hpp"\n')

            f.write(
                '''
std::map<unsigned, Sprite*> Assets::_assets = {};

void Assets::buildAssets() {
  destroyAssets();
''')
            for i in range(len(self._sprites)):
                s = self._sprites[i]
                f.write(
                    f'  _assets.insert({{ASSET_{s.upper()}_ID, new {s}(ASSET_{s.upper()}_HEIGHT, ASSET_{s.upper()}_WIDTH)}});\n')

            f.write(
                '''}

void Assets::destroyAssets() {
  for (const auto& e: _assets) {
    delete e.second;
  }
  _assets.clear();
}

Sprite* Assets::getSpriteById(unsigned id) {
  return _assets.at(id);
}
''')
            f.close()


class GuiAssetsMap:
    def __init__(self,  file: str):
        self._file = file
        self._pack = {}
        self._files = {}
        self._audios = {}

    def addPack(self, pack: str, path: str):
        self._pack[pack] = {
            "path": path,
            "assets": {}
        }

    def addAsset(self, pack: str, asset: str):
        self._pack[pack]["assets"][asset] = [{} for x in range(len(STATES))]

    def addStateSteps(self, pack: str, asset: str, state: int, stateStep: list):
        self._pack[pack]["assets"][asset][state] = stateStep

    def addFile(self, id: int, path: str):
        self._files[id] = path

    def addAudio(self, id: int, paths: list):
        self._audios[id] = paths

    def write(self):
        with open(self._file, 'w') as f:
            f.write(
                '''#include "client/gui/Assets.hpp"

#include <unistd.h>

#include "assetsID.hpp"
#include "client/gui/guiID.hpp"
#include "constants.hpp"

std::vector<std::string> Assets::_pack = {
''')

            for p in self._pack:
                f.write(f'    "{self._pack[p]["path"]}",\n')

            f.write(
                '''};
int Assets::_currentPack = 0;

''')

            f.write(
                f'std::string Assets::_blankAudio = "{BLANK_AUDIO}";\n\n')

            f.write('Assets::AudioMap Assets::_audios = {\n')
            for aid in self._audios:
                f.write(
                    f'    {{\n        PACK_{aid}_ID,\n        {{\n')
                for a in self._audios[aid]:
                    f.write(f'            "{a}",\n')
                f.write('        }\n    },\n')
            f.write('};\n\n')

            f.write('Assets::FileMap Assets::_files = {\n')
            for fid in self._files:
                f.write(
                    f'    {{\n        PACK_{fid}_ID,\n        "{self._files[fid]}",\n    }},\n')
            f.write('};\n\n')

            f.write(
                f'std::string Assets::_blankImage = "{BLANK_IMAGE}";')

            f.write(
                '''

Assets::AssetsMap Assets::_assets = {
''')

            for p in self._pack:
                for a in self._pack[p]["assets"]:
                    f.write(
                        f'    {{\n        ASSET_{a.upper()}_ID,\n        {{\n            ASSET_{a.upper()}_WIDTH,\n            ASSET_{a.upper()}_HEIGHT,\n            {{\n')

                    for s in range(len(self._pack[p]["assets"][a])):
                        f.write('                {\n')

                        for ss in range(len(self._pack[p]["assets"][a][s])):

                            v = ", ".join(
                                [f'"{x}"' for x in self._pack[p]["assets"][a][s][ss]])
                            f.write(
                                f'                    {{{v}}},\n')

                        f.write('                },\n')
                    f.write('            },\n')
                    f.write('        },\n')
                    f.write('    },\n')

            f.write(
                """};

void Assets::setPack(int pack) {
  _currentPack = pack < _pack.size() ? pack : 0;
}

int Assets::getCurrentPack() {
  return _currentPack;
}

std::string Assets::getAudio(int id) {
  std::string path = _pack[_currentPack] + _audios[id][rand() % _audios[id].size()];
  if (access(path.c_str(), F_OK) == -1) return _blankAudio;
  return path;
}

std::string Assets::getFile(int id) {
  std::string path = _pack[_currentPack] + _files[id];
  if (access(path.c_str(), F_OK) == -1) return _blankImage;
  return path;
}

std::vector<Sprite>& Assets::getEditableEntities(std::vector<Sprite>& dest) {
  for (const auto& e: _assets) {
    switch (e.first >> ASSET_LEN_SID) {
      case ASSET_OBSTACLE_TYPE:
      case ASSET_ENEMY_TYPE:
      case ASSET_BOSS_TYPE:
        dest.push_back({e.first, std::get<0>(e.second), std::get<1>(e.second), _pack[_currentPack] + "assets/" + std::get<2>(e.second)[IDLE_STATE][0][0]});
        break;
    }
  }
  return dest;
}

Sprite Assets::getSprite(unsigned id, int state, int stateStep, int variant) {
  int width = std::get<0>(_assets.at(id));
  int height = std::get<1>(_assets.at(id));

  std::vector<std::vector<std::vector<std::string>>> spriteStates = std::get<2>(_assets.at(id));

  if (state >= spriteStates.size()) return {id, width, height, _blankImage};
  std::vector<std::vector<std::string>> spriteSateSteps = spriteStates[state];

  if (spriteSateSteps.size() == 0) {
      spriteSateSteps = spriteStates[0];
  }
  if (stateStep >= spriteSateSteps.size()) stateStep %= spriteSateSteps.size();
  std::vector<std::string> spriteVariant = spriteSateSteps[stateStep];

  if (variant >> ASSET_LEN_SID == ASSET_POWERUP_TYPE) variant = (variant % ASSET_LEN_SID) + 1;
  if (variant >= spriteVariant.size()) variant = 0;

  return {id, width, height, _pack[_currentPack] + "assets/" + spriteVariant[variant]};
}

std::tuple<int, int> Assets::getSpriteSize(unsigned id) {
  int width = std::get<0>(_assets.at(id));
  int height = std::get<1>(_assets.at(id));
  return {width, height};
}""")
            f.close()


def main():
    assetsID = AssetsID(ASSETS_ID_FILE)

    cliAssetsMap = CliAssetsMap(MAP_FILE_CLI)

    for f in sorted(os.listdir(ASSETS_DIR)):
        if not os.path.isdir(os.path.join(ASSETS_DIR, f)):
            filename = f.rsplit('.', 1)[0]

            id = genId(filename)

            if id != None:

                [width, height] = getAssetSize(os.path.join(ASSETS_DIR, f))

                assetsID.add(f'{filename.upper()}_ID', id)
                assetsID.add(f'{filename.upper()}_WIDTH', width)
                assetsID.add(f'{filename.upper()}_HEIGHT', height)

                cliAssetsMap.addSprite(filename)

    assetsID.write()
    cliAssetsMap.write()

    guiID = GuiID(GUI_ID_FILE)
    guiAssetsMap = GuiAssetsMap(MAP_FILE_GUI)

    packs = os.listdir(PACK_DIR)
    defaultPack = packs.index(DEFAULT_PACK)

    packs[defaultPack], packs[0] = packs[0], packs[defaultPack]

    for pack in packs:
        packPath = os.path.join(PACK_DIR, f'{pack}/')
        if os.path.isdir(packPath):
            guiAssetsMap.addPack(pack, packPath)

            gamePath = os.path.join(packPath, "assets")
            for asset in sorted(os.listdir(gamePath)):
                guiAssetsMap.addAsset(pack, asset)

                assetPath = os.path.join(gamePath, asset)
                for state in os.listdir(assetPath):
                    statePath = os.path.join(assetPath, state)

                    stateSteps = []

                    statePath = os.path.join(assetPath, state)
                    for stateStep in sorted(os.listdir(statePath)):

                        stateStepPath = os.path.join(statePath, stateStep)

                        stateSteps.append([os.path.relpath(os.path.join(
                            stateStepPath, variant), gamePath) for variant in sorted(os.listdir(stateStepPath))])

                    guiAssetsMap.addStateSteps(
                        pack, asset, STATES[state], stateSteps)

    defaultPackPath = os.path.join(PACK_DIR, DEFAULT_PACK)

    i = 0
    for a in sorted(os.listdir(os.path.join(defaultPackPath, "audios"))):
        guiID.addAudio(f'AUDIO_{a.upper()}_ID', i)
        guiAssetsMap.addAudio(
            f'AUDIO_{a.upper()}', [os.path.join("audios", a, f) for f in sorted(os.listdir(os.path.join(defaultPackPath, "audios", a)))])
        i += 1

    for b in sorted(os.listdir(os.path.join(defaultPackPath, "background"))):
        filename = b.rsplit('.', 1)[0]
        guiID.add(f'BACKGROUND_{filename.upper()}_ID', i)
        guiAssetsMap.addFile(
            f'BACKGROUND_{filename.upper()}', os.path.join("background", b))
        i += 1

    for h in sorted(os.listdir(os.path.join(defaultPackPath, "hud"))):
        filename = h.rsplit('.', 1)[0]
        guiID.add(f'HUD_{filename.upper()}_ID', i)
        guiAssetsMap.addFile(
            f'HUD_{filename.upper()}', os.path.join("hud", h))
        i += 1

    guiAssetsMap.write()
    guiID.write()


main()
