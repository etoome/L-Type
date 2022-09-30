#include "client/cli/utils.hpp"

#include "constants.hpp"

double mapScaleX() {
  return double(CLI_MAP_WIDTH) / MAP_WIDTH;
}

double mapScaleY() {
  return double(CLI_MAP_HEIGHT) / MAP_HEIGHT;
}

int getKey(WINDOW* win, int xCursor, int yCursor) {
  int c = mvwgetch(win, xCursor, yCursor);
  switch (c) {
    case ESC_SEQ:
      switch (c = mvwgetch(win, xCursor, yCursor)) {
        case ARROW_KEY:
          switch (c = mvwgetch(win, xCursor, yCursor)) {
            case ARROW_UP:
              return KEY_UP;
            case ARROW_DOWN:
              return KEY_DOWN;
            case ARROW_RIGHT:
              return KEY_RIGHT;
            case ARROW_LEFT:
              return KEY_LEFT;
            default:
              return UNKNOWN_KEY;
          }

        default:
          return UNKNOWN_KEY;
      }

    case '\n':
    case 13:
      return KEY_ENTER;
    case 127:
    case '\b':
      return KEY_BACKSPACE;
    case ERR:
      return INVALID_KEY;
    default:
      return c;
  }
}

void hideCursor() {
  noecho();     // Disable built-in echoing
  curs_set(0);  // Disable cursor
}
void showCursor() {
  curs_set(1);  // Enable cursor
  echo();       // Enable built-in echoing
}
