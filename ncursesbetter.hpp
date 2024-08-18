#ifndef NCURSESBETTER_HPP
#define NCURSESBETTER_HPP

#include <string>
#include <ncurses.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

using std::string;

namespace nc {
	void init() {
		initscr();
		noecho();
		curs_set(0);
		start_color();
		use_default_colors();
	}

	void clear() {
		::clear();
	}

	void show() {
		refresh();
	}

	int get_width() {
		int width, height;
		getmaxyx(stdscr, width, height);
		return width;
	}

	int get_height() {
		int width, height;
		getmaxyx(stdscr, width, height);
		return height;
	}

	void set_cell(int x, int y, char character) {
		mvaddch(y, x, character);
	}

	// Returns amount of characters printed
	int print(string text, int x, int y, int width) {
		int i = 0;
		for (i = x; i < text.length(); i++) {
			if (i > width)
				break;

			set_cell(i, y, text[i]);
		}

		return MAX(0, i-x);
	}

	void fini() {
		endwin();
	}
}

#endif // NCURSESBETTER_HPP
