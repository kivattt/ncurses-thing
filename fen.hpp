#ifndef FEN_HPP
#define FEN_HPP

#include <string>

#include "util.hpp"
#include "ncursesbetter.hpp"

#define NCURSES_COLOR 1000/255
#define MYCOLOR_AQUA_PAIR 8
#define MYCOLOR_BLACK_BG_PAIR 9
#define MYCOLOR_LIME_PAIR 10
#define MYCOLOR_USERNAME_PAIR 11

using std::string;

struct TopBar {
	string username;
	string *sel;

	void draw(int x, int y, int width, int height) {
		attron(A_BOLD);

		attron(COLOR_PAIR(MYCOLOR_USERNAME_PAIR));
		int usernameLength = nc::print(username, x, y, width);
		attroff(COLOR_PAIR(MYCOLOR_USERNAME_PAIR));

		attron(COLOR_PAIR(MYCOLOR_AQUA_PAIR));
		nc::print(*sel, x + usernameLength, y, width);
		attroff(COLOR_PAIR(MYCOLOR_AQUA_PAIR));

		attroff(A_BOLD);
	}
};

struct BottomBar {
	void draw(int x, int y, int width, int height) {
		attron(COLOR_PAIR(MYCOLOR_BLACK_BG_PAIR));
		nc::fill_line(x, y, width);
		nc::print("hello world", x, y, width);
		attroff(COLOR_PAIR(MYCOLOR_BLACK_BG_PAIR));
	}
};

struct Fen {
	TopBar topBar;
	BottomBar bottomBar;
	string wd, sel;

	Fen() {
		init_pair(MYCOLOR_AQUA_PAIR, COLOR_BLUE, -1);
		init_pair(MYCOLOR_BLACK_BG_PAIR, -1, COLOR_BLACK);
		init_pair(MYCOLOR_LIME_PAIR, COLOR_GREEN, -1);

		uid_t EUID = geteuid();
		topBar.username = util::get_username(EUID);
		init_pair(MYCOLOR_USERNAME_PAIR, util::get_username_color(EUID), -1);

		topBar.sel = &sel;
		sel = "hello ballers";
	}

	void draw(int x, int y, int width, int height) {
		topBar.draw(x, y, width, height);
		bottomBar.draw(x, height-1, width, height);
	}
};

#endif // FEN_HPP
