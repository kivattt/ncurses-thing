#include <iostream>

#include "history.hpp"
#include "util.hpp"

int main() {
	History history;
	history.add("/home/user/folder1/folder2");

	string e = history.next_path("/home").value();
	if (e != "/home/user") {
		PANIC("Expected /home/user, but got: " + e);
	}

	history.add("/home/user/test/something");
	history.add("/home/user/test.txt");

	e = history.next_path("/home/user/test").value();
	if (e != "/home/user/test/something") {
		PANIC("Expected /home/user/test/something, but got: " + e);
	}

	history.clear();
	history.add("/home/user/folder");
	history.add("/home/user/file.txt");
	e = history.next_path("/home/user").value();
	if (e != "/home/user/file.txt") {
		PANIC("Expected /home/user/file.txt, but got: " + e);
	}
}
