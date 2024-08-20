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
#define MYCOLOR_USERNAME_PAIR 11
#define MYCOLOR_WHITE_PAIR 12
#define MYCOLOR_RED_BG_PAIR 13

using std::string;
using std::vector;
namespace fs = std::filesystem;

struct TopBar {
	string username;
	fs::path *sel;

	void draw(int x, int y, int width, int height) {
		attron(A_BOLD);

		attron(COLOR_PAIR(MYCOLOR_USERNAME_PAIR));
		int usernameLength = nc::print(username, x, y, width) + 1;
		attroff(COLOR_PAIR(MYCOLOR_USERNAME_PAIR));

		attron(COLOR_PAIR(MYCOLOR_AQUA_PAIR));
		int leftPathLength = nc::print(util::path_with_end_slash(sel->parent_path()), x + usernameLength, y, width);
		attroff(COLOR_PAIR(MYCOLOR_AQUA_PAIR));

		attron(COLOR_PAIR(MYCOLOR_WHITE_PAIR));
		nc::print(sel->filename(), x + usernameLength + leftPathLength, y, width);
		attroff(COLOR_PAIR(MYCOLOR_WHITE_PAIR));

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
	int selectedEntryIndex = 0;
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

	string get_entry_from_index(int index) {
		if (index >= entries.size())
			return "";

		if (index < 0)
			return "";

		return entries[index].path().filename();
	}

	// Returns false if entryName not found, true otherwise
	bool set_selected_entry_from_string(string entryName) {
		for (unsigned long long i = 0; i < entries.size(); i++) {
			if (entries[i].path().filename() == entryName) {
				selectedEntryIndex = i;
				return true;
			}
		}

		return false;
	}

	// Returns true if selectedEntryIndex was changed
	bool keep_selection_in_bounds() {
		if (selectedEntryIndex < 0) {
			selectedEntryIndex = 0;
			return true;
		}

		if (selectedEntryIndex >= entries.size()) {
			selectedEntryIndex = MAX(0, entries.size()-1);
			return true;
		}

		return false;
	}

	void draw(int x, int y, int width, int height) {
		if (entries.empty() && folder != "/") {
			attron(COLOR_PAIR(MYCOLOR_RED_BG_PAIR));
			nc::print("empty", x, y, width);
			attroff(COLOR_PAIR(MYCOLOR_RED_BG_PAIR));
			return;
		}

		for (unsigned long long i = 0; i < entries.size(); i++) {
			if (i >= height)
				break;

			if (i == selectedEntryIndex)
				attron(A_REVERSE);

			fs::directory_entry entry = entries[i];
			if (entry.is_directory()) {
				attron(A_BOLD);
				attron(COLOR_PAIR(MYCOLOR_AQUA_PAIR));
			}

			nc::fill_line(x, y+i, width);
			nc::print(entry.path().filename().string(), x+1, y+i, width-1);

			if (i == selectedEntryIndex)
				attroff(A_REVERSE);

			if (entry.is_directory()) {
				attroff(COLOR_PAIR(MYCOLOR_AQUA_PAIR));
				attroff(A_BOLD);
			}
		}
	}
};

struct Fen {
	TopBar topBar;
	BottomBar bottomBar;
	fs::path sel;

	FilesPane leftPane, middlePane, rightPane;

	void update_panes() {
		if (sel.parent_path() != "/") {
			leftPane.folder = sel.parent_path().parent_path();
			leftPane.read_folder();
			leftPane.set_selected_entry_from_string(sel.parent_path().filename());
		} else {
			leftPane.entries.clear();
		}

		middlePane.folder = sel.parent_path();
		middlePane.read_folder();

		/*if (!middlePane.entries.empty())
			sel = middlePane.entries[middlePane.selectedEntryIndex].path();*/

		middlePane.set_selected_entry_from_string(sel.filename());

		rightPane.folder = sel;
		rightPane.read_folder();
	}

	Fen() {
		init_pair(MYCOLOR_AQUA_PAIR, COLOR_BLUE, -1);
		init_pair(MYCOLOR_BLACK_BG_PAIR, -1, COLOR_BLACK);
		init_pair(MYCOLOR_WHITE_PAIR, COLOR_WHITE, -1);
		init_pair(MYCOLOR_RED_BG_PAIR, -1, COLOR_RED);

		uid_t EUID = geteuid();
		topBar.username = util::get_username(EUID);
		init_pair(MYCOLOR_USERNAME_PAIR, util::get_username_color(EUID), -1);

		sel = fs::current_path();
		update_panes();
		if (!middlePane.entries.empty())
			sel = middlePane.entries[0].path();
		update_panes();

		topBar.sel = &sel;
	}

	// Returns true if selection changed
	bool go_left() {
		if (sel.parent_path() != "/") {
			sel = sel.parent_path();
			update_panes();
			return true;
		}
		return false;
	}

	// Returns true if selection changed
	bool go_right() {
		if (rightPane.entries.empty()) {
			return false;
		}
		sel = rightPane.entries[rightPane.selectedEntryIndex];
//		sel = history.get_history_entry_for_path(wd);
		update_panes();
		return true;
	}

	// Returns true if selection changed
	bool go_up() {
		--middlePane.selectedEntryIndex;
		bool selectionWasOutOfBounds = middlePane.keep_selection_in_bounds();
		sel = sel.parent_path() / middlePane.get_entry_from_index(middlePane.selectedEntryIndex);
		update_panes();
		return !selectionWasOutOfBounds;
	}

	// Returns true if selection changed
	bool go_down() {
		++middlePane.selectedEntryIndex;
		bool selectionWasOutOfBounds = middlePane.keep_selection_in_bounds();
		sel = sel.parent_path() / middlePane.get_entry_from_index(middlePane.selectedEntryIndex);
		update_panes();
		return !selectionWasOutOfBounds;
	}

	void draw(int x, int y, int width, int height) {
		topBar.draw(x, y, width, height);

		int leftWidth = double(width) / 6;
		int midAndRightWidth = leftWidth + double(width - leftWidth) / 2;

		leftPane.draw(x, y+1, leftWidth-1, height-1);
		middlePane.draw(leftWidth, y+1, midAndRightWidth-1, height-1);
		rightPane.draw(midAndRightWidth, y+1, width, height-1);

	
		bottomBar.draw(x, height-1, width, height);
	}
};

#endif // FEN_HPP
