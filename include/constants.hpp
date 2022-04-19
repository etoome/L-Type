#pragma once

// General game constants
constexpr unsigned FPS = 30;              // frames/s
constexpr long int TICK = 1000000 / FPS;  // µs/frame
constexpr unsigned FRAMES_BY_LEVEL = FPS * 100;
constexpr unsigned PROGRESS_STEP = 1;       // frames
constexpr long int MAX_LATENCY = 5 * TICK;  // µs

constexpr long int CLIENT_TIMEOUT = 15;  // 

constexpr unsigned MAX_PLAYERS = 2;

constexpr int MAP_WIDTH = 100;
constexpr int MAP_HEIGHT = int(MAP_WIDTH * 9 / 16);

// Game states
constexpr unsigned IDLE_STATE = 0;
constexpr unsigned MOVE_STATE = 1;
constexpr unsigned SHOOT_STATE = 2;
constexpr unsigned HURT_STATE = 3;
constexpr unsigned DIE_STATE = 4;
constexpr unsigned RESPAWN_STATE = 5;
constexpr unsigned PICK_POWER_UP_STATE = 6;

constexpr unsigned STATE_DURATION = FPS * 1;    // frames
constexpr unsigned RESPAWN_DURATION = FPS * 2;  // frames

// Game entity constants
constexpr double ENEMY_HP = 1.0;
constexpr double BULLET_HP = 0.1;
constexpr double OBSTACLE_HP = 1000;
constexpr double BOSS_HP = 3;
constexpr double HENCHMAN_HP = 3;

constexpr double ENEMY_DAMAGE = 1;
constexpr double PLAYER_DAMAGE = 1;
constexpr double ENEMY_MAX_FIRE_DAMAGE = 1;
constexpr double PLAYER_FIRE_DAMAGE = 0.7;
constexpr double OBSTACLE_DAMAGE = 1;

// Note: velocity is in [%map/frame] = MAP_SIZE / ((s/map) * (frames/s))
constexpr double PLAYER_VELOCITY_Y = MAP_HEIGHT / (3.0 * FPS);
constexpr double PLAYER_VELOCITY_X = MAP_WIDTH / (4.0 * FPS);
constexpr double ENEMY_VELOCITY_Y = MAP_HEIGHT / (7.5 * FPS);
constexpr double ENEMY_VELOCITY_X = MAP_WIDTH / (15.0 * FPS);
constexpr double BULLET_VELOCITY = MAP_WIDTH / (2.0 * FPS);
constexpr double OBSTACLE_VELOCITY = MAP_WIDTH / (10.0 * FPS);

// Note: fire delay is in [frame/shoot] = (frames/s) / (shoots/s)
constexpr unsigned PLAYER_FIRE_DELAY = unsigned(FPS / 1.5);
constexpr unsigned ENEMY_FIRE_DELAY = unsigned(FPS / 0.6);
constexpr unsigned BOSS_FIRE_DELAY = unsigned(FPS / 0.75);  // x2
constexpr unsigned HENCHMAN_FIRE_DELAY = unsigned(FPS / 0.8);

constexpr unsigned SCORE_TOUCH_ENEMY = 50;
constexpr unsigned SCORE_KILL_ENEMY = 250;

constexpr double POWERUP_DAMAGE_RATE = 2;
constexpr double POWERUP_FIRE_RATE = 0.75;

constexpr int BOSS_PROGRESS = 95;

// Inputs
constexpr int INVALID_KEY = -2;
constexpr int GAME_KEY_ESC = -1;
constexpr int EMPTY_KEY = 0;
constexpr int GAME_KEY_UP = 1;
constexpr int GAME_KEY_DOWN = 2;
// No 3 (UP+DOWN)
constexpr int GAME_KEY_RIGHT = 4;
constexpr int GAME_KEY_UP_RIGHT = GAME_KEY_UP + GAME_KEY_RIGHT;      // 5
constexpr int GAME_KEY_DOWN_RIGHT = GAME_KEY_DOWN + GAME_KEY_RIGHT;  // 6
// No 7 (UP+DOWN+RIGHT)
constexpr int GAME_KEY_LEFT = 8;
constexpr int GAME_KEY_UP_LEFT = GAME_KEY_UP + GAME_KEY_LEFT;      // 9
constexpr int GAME_KEY_DOWN_LEFT = GAME_KEY_DOWN + GAME_KEY_LEFT;  // 10
// No 11 (LEFT+UP+DOWN)
// No 12 (RIGHT+LEFT)
// No 13 (RIGHT+LEFT+UP)
// No 14 (RIGHT+LEFT+UP+DOWN)
constexpr int GAME_KEY_SHOOT = 15;
constexpr int GAME_KEY_SHOOT_UP = GAME_KEY_SHOOT + GAME_KEY_UP;                  // 16
constexpr int GAME_KEY_SHOOT_DOWN = GAME_KEY_SHOOT + GAME_KEY_DOWN;              // 17
constexpr int GAME_KEY_SHOOT_RIGHT = GAME_KEY_SHOOT + GAME_KEY_RIGHT;            // 19
constexpr int GAME_KEY_SHOOT_UP_RIGHT = GAME_KEY_SHOOT + GAME_KEY_UP_RIGHT;      // 20
constexpr int GAME_KEY_SHOOT_DOWN_RIGHT = GAME_KEY_SHOOT + GAME_KEY_DOWN_RIGHT;  // 21
constexpr int GAME_KEY_SHOOT_LEFT = GAME_KEY_SHOOT + GAME_KEY_LEFT;              // 23
constexpr int GAME_KEY_SHOOT_UP_LEFT = GAME_KEY_SHOOT + GAME_KEY_UP_LEFT;        // 24
constexpr int GAME_KEY_SHOOT_DOWN_LEFT = GAME_KEY_SHOOT + GAME_KEY_DOWN_LEFT;    // 25

constexpr int CHEAT_CODE_LENGHT = 4;
constexpr int CHEAT_CODE_CHAR = '/';
constexpr int CHEAT_CODE_LIFE = 100;
constexpr int CHEAT_CODE_GHOST = 101;
constexpr int CHEAT_CODE_HULK = 102;
constexpr int CHEAT_CODE_SKIP_LEVEL = 103;

// Textual fields
constexpr unsigned ACTIVITYID_LENGTH = 32;
constexpr int USERNAME_MIN = 3;
constexpr int USERNAME_MAX = 16;
constexpr int LEVEL_MIN = 3;
constexpr int LEVEL_MAX = 16;
constexpr int PASSWORD_MIN = 5;

constexpr unsigned MAX_RATING = 5;

// Spash Screen
constexpr int SPLASH_SCREEN_DELAY = 3000;  // ms

// Music
constexpr int VOLUME = 50;              // [0;128]
constexpr int AUDIO_TRANSITION = 1000;  // ms
