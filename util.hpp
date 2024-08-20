#ifndef UTIL_HPP
#define UTIL_HPP

#include <filesystem>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

namespace fs = std::filesystem;

namespace util {
	int get_username_color(uid_t UID) {
		return COLOR_GREEN;

/*		if (UID == 0)
			return COLOR_RED;

		struct passwd *pw = getpwuid(UID);
		if (!pw)
			return COLOR_YELLOW;

		return COLOR_GREEN;*/
	}

	string get_username(uid_t UID) {
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

	string path_with_end_slash(string path) {
		if (path.empty() || path.back() != '/')
			return path + "/";

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
}

#endif // UTIL_HPP
