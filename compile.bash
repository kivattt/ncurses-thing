#!/usr/bin/env bash
unameStr=$(uname)
if [[ $unameStr == 'Astral' ]]; then
	g++ -std=c++23 -O2 main.cpp -o main -lncurses -lcurses -ltinfow
else
	g++ -std=c++23 -O2 main.cpp -o main -lncurses -lcurses
fi
