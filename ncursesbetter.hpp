#ifndef NCURSESBETTER_HPP
#define NCURSESBETTER_HPP

#include <string>
#include <ncurses.h>

using std::string;

namespace nc {
	void init() {
		initscr();
		noecho();
		curs_set(0);
		keypad(stdscr, true);
		start_color();
		use_default_colors();
	}

	void clear() {
		erase();
	}

	void show() {
		refresh();
	}

	int get_width() {
		int width, height;
		getmaxyx(stdscr, height, width);
		return width;
	}

	int get_height() {
		int width, height;
		getmaxyx(stdscr, height, width);
		return height;
	}

	void set_cell(int x, int y, char character) {
		mvaddch(y, x, character);
	}

	// Returns amount of characters printed
	int print(string text, int x, int y, int width) {
		if (x >= width)
			return 0;

		int i = 0;
		for (; i < text.length(); i++) {
			if (x+i >= width)
				break;

			set_cell(x+i, y, text[i]);
		}

		return i;
	}

	// Fills horizontal line at x,y with 'width' number of spaces
	void fill_line(int x, int y, int width) {
		for (int i = x; i < width; i++) {
			set_cell(i, y, ' ');
		}
	}

	void fini() {
		endwin();
	}
}

#endif // NCURSESBETTER_HPP
