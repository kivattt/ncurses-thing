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
		int printCursor;
		nc::with_attr(A_BOLD | COLOR_PAIR(usernameColorPair), [&](){
			printCursor = nc::print(username, x, y, width);
		});

		printCursor += nc::print("@", x + printCursor, y, width);

		attron(A_BOLD);

		nc::with_attr(COLOR_PAIR(MYCOLOR_CYAN_PAIR), [&](){
			printCursor += nc::print(hostname, x + printCursor, y, width);
		});

		++printCursor;

		nc::with_attr(COLOR_PAIR(MYCOLOR_AQUA_PAIR), [&](){
			printCursor += nc::print(util::path_with_end_slash(sel->parent_path()), x + printCursor, y, width);
		});

		nc::with_attr(COLOR_PAIR(MYCOLOR_WHITE_PAIR), [&](){
			nc::print(sel->filename(), x + printCursor, y, width);
		});

		attroff(A_BOLD);
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
			nc::with_attr(COLOR_PAIR(MYCOLOR_RED_BG_PAIR), [&](){
				nc::print("empty", x, y, width);
			});
			return;
		}

		for (unsigned long long i = 0; i < entries.size(); i++) {
			if (i >= height)
				break;

			if (i == selectedEntryIndex)
				attron(A_REVERSE);

			fs::directory_entry entry = entries[i];

			nc::with_attr(util::file_color_attributes(entry), [&](){
				nc::fill_line(' ', x, y+i, width);
				nc::print(entry.path().filename().string(), x+1, y+i, width-1);
			});

			if (i == selectedEntryIndex)
				attroff(A_REVERSE);
		}
	}
};

struct BottomBar {
	fs::path *sel;
	FilesPane *middlePane;

	void draw(int x, int y, int width, int height) {
		// TODO: Maybe use existing directory_entry at some point to minimize stat-ing
		fs::file_status fileStat = fs::status(*sel);

		nc::with_attr(COLOR_PAIR(MYCOLOR_BLACK_BG_PAIR), [&](){
			nc::fill_line(' ', x, y, width);
		});

		int printCursor;
		nc::with_attr(COLOR_PAIR(MYCOLOR_CYAN_BLACK_PAIR), [&](){
			printCursor = nc::print(util::file_permissions_string(fileStat.permissions()), x, y, width) + 1;
		});

		struct stat info;
		stat(sel->string().c_str(), &info);

		nc::with_attr(COLOR_PAIR(util::get_username_colorpair(info.st_uid, true)), [&](){
			printCursor += nc::print(util::file_owner(info) + ":", x + printCursor, y, width);
		});

		nc::with_attr(COLOR_PAIR(util::get_groupname_colorpair(info.st_gid, true)), [&](){
			printCursor += nc::print(util::file_group(info), x + printCursor, y, width) + 1;
		});

		nc::with_attr(COLOR_PAIR(MYCOLOR_BLACK_BG_PAIR), [&](){
			try {
				string dateText = util::time_to_string(fs::last_write_time(*sel));
				printCursor += nc::print(dateText, x + printCursor, y, width);
			} catch (fs::filesystem_error &e) {
				printCursor += nc::print("date unknown", x + printCursor, y, width);
			}

			string indexText = std::to_string(middlePane->selectedEntryIndex + 1) + "/" + std::to_string(middlePane->entries.size());
			nc::print(indexText, width - indexText.size(), y, width);
		});
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
		bottomBar.middlePane = &middlePane;
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
