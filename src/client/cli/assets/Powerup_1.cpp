#include "client/cli/assets/Powerup_1.hpp"

#include "client/cli/assets/Powerup.hpp"
#include "assetsID.hpp"

void Powerup_1::draw(WINDOW* win, int y, int x) const {
  wattron(win, COLOR_PAIR(ASSET_POWERUP_1_ID));
  mvwprintw(win, y, x, "◆");
  wattroff(win, COLOR_PAIR(ASSET_POWERUP_1_ID));
}
