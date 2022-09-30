#pragma once

struct SandboxSettings {
  int levelId = -1;
  char levelName[255];

  SandboxSettings(int id, const std::string& name = ""): levelId(id) {
    strcpy(levelName, name.c_str());
  }
};
