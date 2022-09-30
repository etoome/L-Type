#pragma once

#include <ncurses.h>

#define ESC_SEQ 27
#define UNKNOWN_KEY 0
#define ARROW_KEY 91
#define ARROW_UP 'A'
#define ARROW_DOWN 'B'
#define ARROW_RIGHT 'C'
#define ARROW_LEFT 'D'

constexpr int CLI_MAP_WIDTH = 100;
constexpr int CLI_MAP_HEIGHT = 30;

double mapScaleX();
double mapScaleY();

int getKey(WINDOW*, int xCursor, int yCursor);

void hideCursor();
void showCursor();
