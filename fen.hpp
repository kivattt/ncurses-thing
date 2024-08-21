#ifndef FEN_HPP
#define FEN_HPP

#include <string>
#include <vector>
#include <filesystem>

#include "util.hpp"
#include "ncursesbetter.hpp"

#include "colors.hpp"

using std::string;
using std::vector;
namespace fs = std::filesystem;

struct TopBar {
	string username, hostname;
	int usernameColorPair;
	fs::path *sel;

	void draw(int x, int y, int width, int height) {
		attron(A_BOLD);

		attron(COLOR_PAIR(usernameColorPair));
		//int usernameLength = nc::print(username + "@" + hostname, x, y, width) + 1;
		int usernameLength = nc::print(username, x, y, width);
		attroff(COLOR_PAIR(usernameColorPair));

		attroff(A_BOLD);
		usernameLength += nc::print("@", x + usernameLength, y, width);
		attron(A_BOLD);

		attron(COLOR_PAIR(MYCOLOR_BRIGHTBLUE_PAIR));
		usernameLength += nc::print(hostname, x + usernameLength, y, width);
		attroff(COLOR_PAIR(MYCOLOR_BRIGHTBLUE_PAIR));
		++usernameLength;

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
	fs::path *sel;

	void draw(int x, int y, int width, int height) {
		// TODO: Maybe use existing directory_entry at some point to minimize stat-ing
		fs::file_status fileStat = fs::status(*sel);

		attron(COLOR_PAIR(MYCOLOR_BLACK_BG_PAIR));
		nc::fill_line(x, y, width);
		attroff(COLOR_PAIR(MYCOLOR_BLACK_BG_PAIR));

		attron(COLOR_PAIR(MYCOLOR_BLUE_BLACK_PAIR));
		int bottomBarPrintCursor = nc::print(util::file_permissions_string(fileStat.permissions()), x, y, width) + 1;
		attroff(COLOR_PAIR(MYCOLOR_BLUE_BLACK_PAIR));

		struct stat info;
		stat(sel->string().c_str(), &info);

		attron(COLOR_PAIR(util::get_username_colorpair(info.st_uid, true)));
		bottomBarPrintCursor += nc::print(util::file_owner(info) + ":", x + bottomBarPrintCursor, y, width);
		attroff(COLOR_PAIR(util::get_username_colorpair(info.st_uid, true)));

		attron(COLOR_PAIR(util::get_groupname_colorpair(info.st_gid, true)));
		bottomBarPrintCursor += nc::print(util::file_group(info), x + bottomBarPrintCursor, y, width);
		attroff(COLOR_PAIR(util::get_groupname_colorpair(info.st_gid, true)));
		
		/*
		string fileOwners = util::file_owner(info) + ":" + util::file_group(info);

		attron(COLOR_PAIR(MYCOLOR_GREEN_BLACK_PAIR));
		nc::print(fileOwners, x+filePermsLength, y, width);
		attroff(COLOR_PAIR(MYCOLOR_GREEN_BLACK_PAIR));*/
	}
};

struct FilesPane {
	bool isLeftFilesPane = false;
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
		if (entries.empty() && folder != "/" && !isLeftFilesPane) {
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
			util::file_color_attron(entry);

			nc::fill_line(x, y+i, width);
			nc::print(entry.path().filename().string(), x+1, y+i, width-1);

			util::file_color_attroff(entry);

			if (i == selectedEntryIndex)
				attroff(A_REVERSE);
		}
	}
};

struct Fen {
	TopBar topBar;
	BottomBar bottomBar;
	fs::path sel;

	FilesPane leftPane, middlePane, rightPane;

	void update_panes(bool forceReadDir = false) {
		if (sel.parent_path() != "/") {
			leftPane.folder = sel.parent_path().parent_path();
			if (forceReadDir)
				leftPane.read_folder();
			leftPane.set_selected_entry_from_string(sel.parent_path().filename());
		} else {
			leftPane.entries.clear();
		}

		middlePane.folder = sel.parent_path();
		if (forceReadDir)
			middlePane.read_folder();

		/*if (!middlePane.entries.empty())
			sel = middlePane.entries[middlePane.selectedEntryIndex].path();*/

		middlePane.set_selected_entry_from_string(sel.filename());

		rightPane.folder = sel;
		// TODO: Don't read rightpane folder when nothing happened
//		if (forceReadDir)
			rightPane.read_folder();
	}

	Fen() {
		init_pair(MYCOLOR_AQUA_PAIR, COLOR_BLUE, -1);
		init_pair(MYCOLOR_BLACK_BG_PAIR, -1, COLOR_BLACK);
		init_pair(MYCOLOR_WHITE_PAIR, COLOR_WHITE, -1);
		init_pair(MYCOLOR_RED_BG_PAIR, -1, COLOR_RED);
		init_pair(MYCOLOR_BRIGHTBLUE_PAIR, COLOR_CYAN, -1);
		init_pair(MYCOLOR_BLUE_BLACK_PAIR, COLOR_BLUE, COLOR_BLACK);

		init_pair(MYCOLOR_GREEN_PAIR, COLOR_GREEN, -1);
		init_pair(MYCOLOR_RED_PAIR, COLOR_RED, -1);
		init_pair(MYCOLOR_YELLOW_PAIR, COLOR_YELLOW, -1);

		init_pair(MYCOLOR_GREEN_BLACK_PAIR, COLOR_GREEN, COLOR_BLACK);
		init_pair(MYCOLOR_RED_BLACK_PAIR, COLOR_RED, COLOR_BLACK);
		init_pair(MYCOLOR_YELLOW_BLACK_PAIR, COLOR_YELLOW, COLOR_BLACK);


		uid_t EUID = geteuid();
		topBar.username = util::get_username(EUID);
		topBar.hostname = util::get_hostname();
		topBar.usernameColorPair = util::get_username_colorpair(EUID);

		leftPane.isLeftFilesPane = true;

		sel = fs::current_path();
		update_panes(true);
		if (!rightPane.entries.empty())
			sel = rightPane.entries[0].path();

		update_panes(true); // Maybe false here?

		topBar.sel = &sel;
		bottomBar.sel = &sel;
	}

	// Returns true if selection changed
	bool go_left() {
		if (sel.parent_path() != "/") {
			sel = sel.parent_path();
			update_panes(true);
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
		update_panes(true);
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

	// Returns true if selection changed
	bool go_top() {
		if (middlePane.selectedEntryIndex == 0)
			return false;

		middlePane.selectedEntryIndex = 0;
		sel = sel.parent_path() / middlePane.get_entry_from_index(middlePane.selectedEntryIndex);
		update_panes();
		return true;
	}

	// Returns true if selection changed
	bool go_bottom() {
		if (middlePane.selectedEntryIndex == MAX(0, middlePane.entries.size()-1))
			return false;

		middlePane.selectedEntryIndex = MAX(0, middlePane.entries.size()-1);
		sel = sel.parent_path() / middlePane.get_entry_from_index(middlePane.selectedEntryIndex);
		update_panes();
		return true;
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
