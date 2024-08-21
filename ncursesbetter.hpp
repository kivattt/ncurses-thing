#ifndef NCURSESBETTER_HPP
#define NCURSESBETTER_HPP

#include <string>
#include <functional>
#include <ncurses.h>

using std::string;

namespace nc {
	void init() {
		initscr();
		noecho();
		curs_set(0);
		keypad(stdscr, true);
		mousemask(ALL_MOUSE_EVENTS, NULL);
		start_color();
		use_default_colors();
	}

	void clear() {
		erase();
	}

	// Re-draws everything from scratch in the next nc::show() call, to fix any possible broken output
	void sync() {
		::clear();
	}

	void show() {
		refresh();
	}

	void with_attr(const int ncursesAttributes, std::function<void ()> fn) {
		attron(ncursesAttributes);
		fn();
		attroff(ncursesAttributes);
	}

	// Returns terminal width
	int get_width() {
		int width, height;
		getmaxyx(stdscr, height, width);
		return width;
	}

	// Returns terminal height
	int get_height() {
		int width, height;
		getmaxyx(stdscr, height, width);
		return height;
	}

	void set_cell(const int x, const int y, const chtype character) {
		mvaddch(y, x, character);
	}

	// Draws text at x,y up to xMax-1
	// Returns amount of characters printed
	int print(const string &text, const int x, const int y, const int xMax) {
		if (x >= xMax)
			return 0;

		int i = 0;
		for (; i < text.length(); i++) {
			if (x+i >= xMax)
				break;

			set_cell(x+i, y, text[i]);
		}

		return i;
	}

	// Fills horizontal line with the specified character at x,y up to xMax-1
	void fill_line(const chtype character, const int x, const int y, const int xMax) {
		for (int i = x; i < xMax; i++)
			set_cell(i, y, character);
	}

	void fini() {
		endwin();
	}
}

#endif // NCURSESBETTER_HPP
