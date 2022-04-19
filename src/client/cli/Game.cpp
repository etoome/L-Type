#include "client/cli/Game.hpp"

#include "client/KeyMapping.hpp"
#include "utils.hpp"

Game::CheatCodes Game::_cheatCodes = {
    {
        CHEAT_CODE_LIFE,
        {CHEAT_CODE_CHAR, 'c', 'h', 'a'},
    },
    {
        CHEAT_CODE_GHOST,
        {CHEAT_CODE_CHAR, 'j', 'a', 'c'},
    },
    {
        CHEAT_CODE_HULK,
        {CHEAT_CODE_CHAR, 'r', 'o', 'b'},
    },
    {
        CHEAT_CODE_SKIP_LEVEL,
        {CHEAT_CODE_CHAR, 'c', 'e', 'd'},
    },
};

Game::Game(const std::array<std::size_t, MAX_PLAYERS>& mapping) noexcept {
  for (std::size_t p = 0; p != MAX_PLAYERS; ++p) {
    _userMapping[p] = mapping[p];
  }

  for (std::size_t c = 0; c != CHEAT_CODE_LENGHT; ++c) {
    _lastInputs.push_back(INVALID_KEY);
  }
}

int Game::getInput() noexcept {
  int key = _gameViewport->getInputKey();

  if (key == INVALID_KEY) {
    return INVALID_KEY;
  }
  if (KeyMapping::matches(key, GAME_KEY_ESC)) {
    return GAME_KEY_ESC;
  }

  // Cheat codes
  _lastInputs.pop_front();
  _lastInputs.push_back(key);
  if (_lastInputs.front() == CHEAT_CODE_CHAR) {
    for (const CheatCodes::value_type& cheatCode: _cheatCodes) {
      int ret = cheatCode.first;

      for (size_t i = 0; i != cheatCode.second.size(); ++i) {
        if (_lastInputs[i] != cheatCode.second[i]) {
          ret = 0;
          break;
        }
      }

      if (ret != 0) {
        return cheatCode.first;
      }
    }
  }

  int input = INVALID_KEY;
  for (const KeyMapping::Mapping::value_type& mapping: KeyMapping::get()) {
    int mapKey1 = mapping.second[_userMapping[0]];
    int mapKey2 = mapping.second[_userMapping[1]];

    if (key == mapKey1 || key == mapKey2) {
      int p = mapKey1 != key;

      input = mapping.first;

      input <<= 1;
      input += int(p);

      break;
    }
  }

  if (input == INVALID_KEY && _lastInputTimestamp != 0 && (getTimestamp() - _lastInputTimestamp) / 1000000 > CLIENT_TIMEOUT - 5) {
    _lastInputTimestamp = getTimestamp();
    input = EMPTY_KEY;
  } else {
    _lastInputTimestamp = getTimestamp();
  }

  return input;
}

void Game::updateGameState(unsigned progress, const unsigned score[2], const double hpPlayers[2]) {
  _progress = progress;
  _score[0] = score[0];
  _score[1] = score[1];
  _hpPlayers[0] = hpPlayers[0];
  _hpPlayers[1] = hpPlayers[1];
}

void Game::clearEntities() {
  _entities.clear();
}

void Game::addEntity(const Entity& entity) {
  _entities.push_back(entity);
}

void Game::drawWindow() {
  _gameViewport->drawGame(_score, _hpPlayers, _progress, _entities);
}
