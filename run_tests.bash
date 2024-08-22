#!/usr/bin/env bash
testFolder=./tests-bin

if [ ! -d $testFolder ]; then
	mkdir $testFolder
fi

unameStr=$(uname)
if [[ $unameStr == 'Astral' ]]; then
	g++ -std=c++23 -O2 history_test.cpp -o $testFolder/history_test -lncurses -lcurses -ltinfow
else
	g++ -std=c++23 -O2 history_test.cpp -o $testFolder/history_test -lncurses -lcurses
fi

./$testFolder/history_test
