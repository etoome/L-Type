#include "client/cli/assets/Powerup_2.hpp"

#include "client/cli/assets/Powerup.hpp"
#include "assetsID.hpp"

void Powerup_2::draw(WINDOW* win, int y, int x) const {
  wattron(win, COLOR_PAIR(ASSET_POWERUP_2_ID));
  mvwprintw(win, y, x, "◆");
  wattroff(win, COLOR_PAIR(ASSET_POWERUP_2_ID));
}
