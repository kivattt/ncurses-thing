#include <iostream>
#include <ncurses.h>

#include "ncursesbetter.hpp"
#include "fen.hpp"

int main() {
	std::cout << "\x1b[22t"; // Push current terminal title
	std::cout << "\x1b]0;2.5fen\x07"; // Set terminal title to "2.5fen"

	nc::init();

	Fen fen;

	bool redraw = true;
	bool running = true;
	while (running) {
		if (redraw) {
			nc::clear();
			fen.draw(0, 0, nc::get_width(), nc::get_height());
			nc::show();
		}
		redraw = true;

		int ch = getch();
		switch (ch) {
			case 'q':
				running = false;
				break;
			case 'h':
			case KEY_LEFT:
				redraw = fen.go_left();
				break;
			case 'l':
			case KEY_RIGHT:
				redraw = fen.go_right();
				break;
			case 'k':
			case KEY_UP:
				redraw = fen.go_up();
				break;
			case 'j':
			case KEY_DOWN:
				redraw = fen.go_down();
				break;
			default:
				redraw = false;
				break;
		}
	}

	nc::fini();
	std::cout << "\x1b[23t"; // Pop terminal title, sets it back to normal
}
