#pragma once

#include <tuple>

#include "assetsID.hpp"

constexpr int PLAYER_INFO_ID = 0;
constexpr int PLAYER_INFO_WIDTH = 1;
constexpr int PLAYER_INFO_HEIGHT = 2;

std::array<std::tuple<unsigned, int, int>, 5> playerInfo = {
    {
        {ASSET_PLAYER_1_ID, ASSET_PLAYER_1_WIDTH, ASSET_PLAYER_1_HEIGHT},
        {ASSET_PLAYER_2_ID, ASSET_PLAYER_2_WIDTH, ASSET_PLAYER_2_HEIGHT},
        {ASSET_PLAYER_3_ID, ASSET_PLAYER_3_WIDTH, ASSET_PLAYER_3_HEIGHT},
        {ASSET_PLAYER_4_ID, ASSET_PLAYER_4_WIDTH, ASSET_PLAYER_4_HEIGHT},
        {ASSET_PLAYER_5_ID, ASSET_PLAYER_5_WIDTH, ASSET_PLAYER_5_HEIGHT},
    },
};
