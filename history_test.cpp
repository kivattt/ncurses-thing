#include <iostream>

#include "history.hpp"
#include "util.hpp"

int main() {
	History history;
	history.add("/home/user/folder1/folder2");

	string e = history.get_history_entry_for_path("/home");
	if (e != "/home/user") {
		PANIC("Expected /home/user, but got: " + e);
	}

	history.add("/home/user/test/something");
	history.add("/home/user/test.txt");

	e = history.get_history_entry_for_path("/home/user/test");
	if (e != "/home/user/test/something") {
		PANIC("Expected /home/user/test/something, but got: " + e);
	}
}
