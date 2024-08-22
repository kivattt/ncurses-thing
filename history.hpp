#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <vector>
#include <string>
#include <filesystem>

using std::vector;
using std::string;
namespace fs = std::filesystem;

struct History {
	vector<string> history;

	string get_history_entry_for_path(const string path) {
		for (string &e : history) {
			fs::path sub = fs::relative(e, path);
			if (e.starts_with(path)) {
				if (path.length() >= e.length())
					continue;


			}
		}
	}

	void add(const string path) {
		vector<string>::iterator it = std::find(history.begin(), history.end(), path);

		if (it != history.end()) {
			if (history.front() == path)
				return;

			// Found in the history, but we need to move it to the front again
			history.erase(it);
			history.insert(history.begin(), path);
			return;
		}

		history.insert(history.begin(), path);
	}

	void remove(const string path) {

	}
};

#endif // HISTORY_HPP
