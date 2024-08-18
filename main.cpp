#include <iostream>
#include <ncurses.h>

#include "ncursesbetter.hpp"
#include "fen.hpp"

int main() {
	nc::init();

	Fen fen;

	bool running = true;
	while (running) {
		nc::clear();
		fen.draw(0, 0, nc::get_width(), nc::get_height());
		nc::show();
		char ch = getch();

		if (ch == 'q') {
			running = false;
			break;
		}
	}

	nc::fini();
}
