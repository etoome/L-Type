#include "client/cli/assets/Henchman_1_left.hpp"

void Henchman_1_left::draw(WINDOW* win, int y, int x) const {
  wattron(win, COLOR_PAIR(4));
  mvwprintw(win, y + 0, x, "  / /   ");
  mvwprintw(win, y + 1, x, " / /    ");
  mvwprintw(win, y + 2, x, "( (´    ");
  mvwprintw(win, y + 3, x, " \\ \\    ");
  mvwprintw(win, y + 4, x, "  '.'-. ");
  mvwprintw(win, y + 5, x, "    ´) )");
  mvwprintw(win, y + 6, x, "    `-´ ");
  wattroff(win, COLOR_PAIR(4));
}
