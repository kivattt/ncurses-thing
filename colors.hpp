#ifndef COLORS_HPP
#define COLORS_HPP

#include <ncurses.h>

#define NCURSES_COLOR 1000/255
#define MYCOLOR_AQUA_PAIR 8
#define MYCOLOR_BLACK_BG_PAIR 9
#define MYCOLOR_USERNAME_PAIR 11
#define MYCOLOR_WHITE_PAIR 12
#define MYCOLOR_RED_BG_PAIR 13
#define MYCOLOR_CYAN_PAIR 14
#define MYCOLOR_CYAN_BLACK_PAIR 15

#define MYCOLOR_GREEN_PAIR 17
#define MYCOLOR_RED_PAIR 18
#define MYCOLOR_YELLOW_PAIR 19

#define MYCOLOR_GREEN_BLACK_PAIR 20
#define MYCOLOR_RED_BLACK_PAIR 21
#define MYCOLOR_YELLOW_BLACK_PAIR 22

// This has to be called after ncurses is initialized
void init_all_color_pairs() {
	init_pair(MYCOLOR_AQUA_PAIR, COLOR_BLUE, -1);
	init_pair(MYCOLOR_BLACK_BG_PAIR, -1, COLOR_BLACK);
	init_pair(MYCOLOR_WHITE_PAIR, COLOR_WHITE, -1);
	init_pair(MYCOLOR_RED_BG_PAIR, -1, COLOR_RED);
	init_pair(MYCOLOR_CYAN_PAIR, COLOR_CYAN, -1);
	init_pair(MYCOLOR_CYAN_BLACK_PAIR, COLOR_CYAN, COLOR_BLACK);

	init_pair(MYCOLOR_GREEN_PAIR, COLOR_GREEN, -1);
	init_pair(MYCOLOR_RED_PAIR, COLOR_RED, -1);
	init_pair(MYCOLOR_YELLOW_PAIR, COLOR_YELLOW, -1);

	init_pair(MYCOLOR_GREEN_BLACK_PAIR, COLOR_GREEN, COLOR_BLACK);
	init_pair(MYCOLOR_RED_BLACK_PAIR, COLOR_RED, COLOR_BLACK);
	init_pair(MYCOLOR_YELLOW_BLACK_PAIR, COLOR_YELLOW, COLOR_BLACK);
}

#endif // COLORS_HPP
