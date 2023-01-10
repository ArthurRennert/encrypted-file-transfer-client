/**
 * Encrypted File Transfer Client
 * @file main.cpp
 * @brief Client program entry point.
 * Compiled with: Visual Studio 2022. C++14.
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

	for (;;)
	{
		menu.display();
		menu.handleUserChoice();
		menu.pause();
	}

	return 0;
}

