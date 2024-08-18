#ifndef UTIL_HPP
#define UTIL_HPP

#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

namespace util {
	int get_username_color(uid_t UID) {
		if (UID == 0)
			return COLOR_RED;

		struct passwd *pw = getpwuid(UID);
		if (!pw)
			return COLOR_YELLOW;

		return COLOR_GREEN;
	}

	string get_username(uid_t UID) {
		struct passwd *pw = getpwuid(UID);
		if (pw)
			return pw->pw_name;
		return "unknown";
	}
}

#endif // UTIL_HPP
