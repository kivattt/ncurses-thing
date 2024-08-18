#ifndef FEN_HPP
#define FEN_HPP

#include <string>
#include <vector>
#include <filesystem>

#include "util.hpp"
#include "ncursesbetter.hpp"

#define NCURSES_COLOR 1000/255
#define MYCOLOR_AQUA_PAIR 8
#define MYCOLOR_BLACK_BG_PAIR 9
#define MYCOLOR_LIME_PAIR 10
#define MYCOLOR_USERNAME_PAIR 11

using std::string;
using std::vector;
namespace fs = std::filesystem;

struct TopBar {
	string username;
	fs::path *sel;

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

struct FilesPane {
	vector <fs::directory_entry> entries;
	string folder;

	// Updates the entries
	void read_folder() {
		entries.clear();
		try {
			for (const fs::directory_entry &entry : fs::directory_iterator(folder))
				entries.push_back(entry);
		} catch (fs::filesystem_error &e) {
			return;
		}
	}

	void draw(int x, int y, int width, int height) {
		for (unsigned long long i = 0; i < entries.size(); i++) {
			if (i > height)
				break;

			fs::directory_entry entry = entries[i];
			nc::print(entry.path().filename(), x, y+i, width);
		}
	}
};

struct Fen {
	TopBar topBar;
	BottomBar bottomBar;
	fs::path wd, sel;

	FilesPane leftPane, middlePane, rightPane;

	void update_panes() {
		leftPane.folder = wd.parent_path();
		leftPane.read_folder();

		middlePane.folder = wd;
		middlePane.read_folder();

		if (!middlePane.entries.empty())
			sel = middlePane.entries[0].path();

		rightPane.folder = sel.parent_path();
		rightPane.read_folder();
	}

	Fen() {
		init_pair(MYCOLOR_AQUA_PAIR, COLOR_BLUE, -1);
		init_pair(MYCOLOR_BLACK_BG_PAIR, -1, COLOR_BLACK);
		init_pair(MYCOLOR_LIME_PAIR, COLOR_GREEN, -1);

		uid_t EUID = geteuid();
		topBar.username = util::get_username(EUID);
		init_pair(MYCOLOR_USERNAME_PAIR, util::get_username_color(EUID), -1);

		wd = fs::current_path();
		update_panes();
		if (!middlePane.entries.empty())
			sel = middlePane.entries[0].path();
		update_panes();

		topBar.sel = &sel;
	}

	void go_left() {
		sel = wd;
		wd = wd.parent_path();
		update_panes();
	}

	void go_right() {
		wd = sel;
		update_panes();
//		sel = history.get_history_entry_for_path(wd);
	}

	void draw(int x, int y, int width, int height) {
		topBar.draw(x, y, width, height);

		int leftWidth = double(width) / 6;
		int midAndRightWidth = double(width - leftWidth) / 2;

		leftPane.draw(x, y+1, leftWidth, height-1);
		middlePane.draw(leftWidth, y+1, midAndRightWidth, height-1);
		rightPane.draw(midAndRightWidth, y+1, width, height-1);

		bottomBar.draw(x, height-1, width, height);
	}
};

#endif // FEN_HPP
