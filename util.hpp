#ifndef UTIL_HPP
#define UTIL_HPP

#include <filesystem>
#include <chrono>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "colors.hpp"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define PANIC(msg) { \
		std::cerr << (msg) << std::endl; \
		std::exit(EXIT_FAILURE); \
	}

namespace fs = std::filesystem;

namespace util {
	int get_username_colorpair(const uid_t UID, const bool withBlackBackground = false) {
		if (UID == 0)
			return withBlackBackground ? MYCOLOR_RED_BLACK_PAIR : MYCOLOR_RED_PAIR;

		struct passwd *pw = getpwuid(UID);
		if (!pw)
			return withBlackBackground ? MYCOLOR_YELLOW_BLACK_PAIR : MYCOLOR_YELLOW_PAIR;

		return withBlackBackground ? MYCOLOR_GREEN_BLACK_PAIR : MYCOLOR_GREEN_PAIR;
	}

	int get_groupname_colorpair(const gid_t GID, const bool withBlackBackground = false) {
		struct group *gr = getgrgid(GID);
		if (!gr)
			return withBlackBackground ? MYCOLOR_YELLOW_BLACK_PAIR : MYCOLOR_YELLOW_PAIR;

		if (GID == 0)
			return withBlackBackground ? MYCOLOR_RED_BLACK_PAIR : MYCOLOR_RED_PAIR;
		return withBlackBackground ? MYCOLOR_GREEN_BLACK_PAIR : MYCOLOR_GREEN_PAIR;
	}

	string get_username(const uid_t UID) {
		struct passwd *pw = getpwuid(UID);
		if (pw)
			return pw->pw_name;
		return "unknown";
	}

	string get_hostname() {
		char hostname[HOST_NAME_MAX + 1];
		gethostname(hostname, HOST_NAME_MAX + 1);
		return hostname;
	}

	string get_home_directory() {
		const char *ret;
		ret = getenv("HOME");
		if (ret != NULL)
			return ret;

		struct passwd *pw = getpwuid(geteuid());
		if (pw)
			return pw->pw_dir;

		PANIC("Failed to find home directory from $HOME or getpwuid()");
	}

	string path_with_trailing_separator(const string path) {
		if (path.empty() || path.back() != '/')
			return path + "/";

		return path;
	}

	string path_without_trailing_separator(string path) {
		if (path.empty() || path.back() != '/')
			return path;

		path.pop_back();
		return path;
	}

	string path_with_home_as_tilde(const string path) {
		string homeDir = get_home_directory();
		if (path.starts_with(homeDir))
			return "~" + path.substr(homeDir.length());

		return path;
	}

	string file_permissions_string(fs::perms permissions) {
		string ret = "";

		const string permissionChars = "xwr";
		for (int i = 8; i >= 0; i--)
			ret += fs::perms::none == (fs::perms(1 << i) & permissions) ? '-' : permissionChars[i%3];

		return ret;
	}

	string file_owner(struct stat &info) {
		struct passwd *pw = getpwuid(info.st_uid);
		if (pw)
			return pw->pw_name;
		return "unknown";
	}

	string file_group(struct stat &info) {
		struct group *gr = getgrgid(info.st_gid);
		if (gr)
			return gr->gr_name;
		return "unknown";
	}

	int file_color_attributes(const fs::directory_entry &entry) {
		if (entry.is_directory())
			return A_BOLD | COLOR_PAIR(MYCOLOR_AQUA_PAIR);

		fs::file_status fileStat = fs::status(entry);
		if ((fileStat.permissions() & fs::perms(0b1001001)) != fs::perms::none)
			return A_BOLD | COLOR_PAIR(MYCOLOR_GREEN_PAIR);

		return A_NORMAL;
	}

	string time_to_string(const fs::file_time_type &ftime) {
		std::time_t cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(ftime));
		std::tm *localTime = std::localtime(&cftime);
		if (localTime == nullptr)
			return "date unknown";

		char buf[256];
		std::strftime(buf, sizeof(buf), "%c", localTime);
		return buf;
	}
}

#endif // UTIL_HPP
