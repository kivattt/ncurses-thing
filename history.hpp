#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <filesystem>

using std::vector;
using std::string;
namespace fs = std::filesystem;

class History {
	private:
	vector<string> history;

	public:

	std::optional<string> next_path(const string path) {
		for (string e : history) {
			fs::path sub = fs::relative(e, path);
			if (sub.string().starts_with(".."))
				continue;

//			if (e.starts_with(path+"/")) {
			if (e.starts_with(path)) {
				if (path.length() >= e.length())
					continue;

				e = e.substr(path.length()+1);
				std::size_t nextSlashIndex = e.find('/');
				if (nextSlashIndex == string::npos) {
					return fs::path(path) / fs::path(e);
				}

				return fs::path(path) / e.substr(0, nextSlashIndex);
			}
		}

		return {};
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
		vector<string>::iterator it = std::find(history.begin(), history.end(), path);

		if (it != history.end())
			history.erase(it);
	}

	void clear() {
		history.clear();
	}
};

#endif // HISTORY_HPP
