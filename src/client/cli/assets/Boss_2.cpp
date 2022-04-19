#include "client/cli/assets/Boss_2.hpp"

void Boss_2::draw(WINDOW* win, int y, int x) const {
    wattron(win,COLOR_PAIR(5));
    mvwprintw(win,y,x+9,"_");
    mvwprintw(win,y,x+10,"_");

    mvwprintw(win,y+1,x+7,"_");
    mvwprintw(win,y+1,x+8,"|");
    mvwprintw(win,y+1,x+11,"|");
    mvwprintw(win,y+1,x+12,"_");

    mvwprintw(win,y+2,x+5,"_");
    mvwprintw(win,y+2,x+6,"|");
    mvwprintw(win,y+2,x+13,"|");
    mvwprintw(win,y+2,x+14,"_");

    mvwprintw(win,y+3,x+4,"|");
    mvwprintw(win,y+3,x+7,"_");
    mvwprintw(win,y+3,x+12,"|");
    mvwprintw(win,y+3,x+15,"|");

    mvwprintw(win,y+4,x+4,"|");
    mvwprintw(win,y+4,x+6,"|");
    mvwprintw(win,y+4,x+7,"*");
    mvwprintw(win,y+4,x+8,"|");
    mvwprintw(win,y+4,x+11,"|");
    mvwprintw(win,y+4,x+12,"*");
    mvwprintw(win,y+4,x+13,"|");
    mvwprintw(win,y+4,x+15,"|");

    mvwprintw(win,y+5,x+1,"_");
    mvwprintw(win,y+5,x+4,"|");
    mvwprintw(win,y+5,x+7,"_");
    mvwprintw(win,y+5,x+12,"_");
    mvwprintw(win,y+5,x+15,"|");
    mvwprintw(win,y+5,x+17,"_");

    mvwprintw(win,y+6,x,"|");
    mvwprintw(win,y+6,x+1,"_");
    mvwprintw(win,y+6,x+2,"|");
    mvwprintw(win,y+6,x+3,"_");
    mvwprintw(win,y+6,x+4,"|");
    mvwprintw(win,y+6,x+5,"_");
    mvwprintw(win,y+6,x+6,"|");
    mvwprintw(win,y+6,x+8,"|");
    mvwprintw(win,y+6,x+9,"_");
    mvwprintw(win,y+6,x+10,"_");
    mvwprintw(win,y+6,x+11,"|");
    mvwprintw(win,y+6,x+13,"|");
    mvwprintw(win,y+6,x+14,"_");
    mvwprintw(win,y+6,x+15,"|");
    mvwprintw(win,y+6,x+16,"_");
    mvwprintw(win,y+6,x+17,"|");
    mvwprintw(win,y+6,x+18,"_");
    mvwprintw(win,y+6,x+19,"|");

    mvwprintw(win,y+7,x+2,"|");
    mvwprintw(win,y+7,x+3,"_");
    mvwprintw(win,y+7,x+4,"|");
    mvwprintw(win,y+7,x+5,"_");
    mvwprintw(win,y+7,x+14,"_");
    mvwprintw(win,y+7,x+15,"|");
    mvwprintw(win,y+7,x+16,"_");
    mvwprintw(win,y+7,x+17,"|");

    mvwprintw(win,y+8,x+4,"|");
    mvwprintw(win,y+8,x+5,"_");
    mvwprintw(win,y+8,x+6,"|");
    mvwprintw(win,y+8,x+13,"|");
    mvwprintw(win,y+8,x+14,"_");
    mvwprintw(win,y+8,x+15,"|");
    wattroff(win, COLOR_PAIR(5));
}
