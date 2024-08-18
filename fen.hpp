#ifndef FEN_HPP
#define FEN_HPP

#include <string>

#include "ncursesbetter.hpp"

#define NCURSES_COLOR 1000/255

#define COLOR_AQUA 8
#define COLOR_AQUA_PAIR 8

using std::string;

struct TopBar {
	string *sel;

	void draw(int x, int y, int width, int height) {
		attron(COLOR_PAIR(COLOR_AQUA_PAIR));
		attron(A_BOLD);
		nc::print(*sel, x, y, width);
		attroff(COLOR_PAIR(COLOR_AQUA_PAIR));
		attroff(A_BOLD);
	}
};

struct Fen {
	TopBar topBar;
	string wd, sel;

	Fen() {
		init_color(COLOR_AQUA, 10 * NCURSES_COLOR, 157 * NCURSES_COLOR, 246 * NCURSES_COLOR);
		init_pair(COLOR_AQUA_PAIR, COLOR_AQUA, -1);

		topBar.sel = &sel;
		sel = "hello ballers";
	}

	void draw(int x, int y, int width, int height) {
		topBar.draw(x, y, width, height);
	}
};

#endif // FEN_HPP
