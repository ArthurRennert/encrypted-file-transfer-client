/**
 * Encrypted File Transfer Client
 * @file main.cpp
 * @brief Client program entry point.
 * Compiled with: Visual Studio 2022. C++20.
 * Multi-threaded Debug (/MTd).
 * Boost Library 1.81.0 (static linkage).
 * Crypto++ Library 8.7 (static linkage).
 * @author Arthur Rennert
 */

#include "pch.h"
#include "ClientMenu.h"

int main(int argc, char* argv[])
{
	ClientMenu menu;
	menu.initialize();

	while(true)
	{
		menu.display();
		menu.handleUserChoice();
		menu.pause();
	}

	return 0;
}

