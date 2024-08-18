#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <vector>
#include <filesystem>

using std::vector;
namespace fs = std::filesystem;

struct History {
	vector<fs::path> history;

	fs::path get_history_entry_for_path(fs::path path) {
		for (fs::path &e : history) {
			fs::path sub = fs::relative(e, path);
			if (e.starts_with(path)) {
				if (path.length() >= e.length())
					continue;


			}
		}
	}
};

#endif // HISTORY_HPP
