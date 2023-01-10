/**
 * Encrypted File Transfer Client
 * @file ClientMenu.cpp
 * @brief Interface class for user input. Handle user's requests.
 * can be replaced by GUI class and invoke CClientLogic correspondingly.
 * @author Arthur Rennert
 */
#include "pch.h"
#include "ClientMenu.h"
#include <iostream>
#include <boost/algorithm/string/trim.hpp>

 /**
  * Print error and exit client.
  */
void ClientMenu::clientStop(const std::string& error) const
{
	std::cout << "Fatal Error: " << error << std::endl << "Client will stop." << std::endl;
	pause();
	exit(1);
}

/**
 * Initialize client's menu & its internals.
 */
void ClientMenu::initialize()
{
	if (!_clientLogic.parseServeInfo())
	{
		clientStop(_clientLogic.getLastError());
	}
	_registered = _clientLogic.parseClientInfo();

}

/**
 * Print main menu to the screen.
 */
void ClientMenu::display() const
{
	clear();
	if (_registered && !_clientLogic.getSelfUsername().empty())
		std::cout << "Hello " << _clientLogic.getSelfUsername() << ", ";
	std::cout << "*** Encrypted File Transfer ***\n\nChoose an option from the menu below:" << std::endl << std::endl;
	for (const auto& opt : _menuOptions)
		std::cout << opt << std::endl;
}


/**
 * Read input from console.
 * Do not allow empty lines.
 */
std::string ClientMenu::readUserInput(const std::string& description) const
{
	std::string input;
	std::cout << description << std::endl;
	do
	{
		std::getline(std::cin, input);
		boost::algorithm::trim(input);
		if (std::cin.eof())   // ignore ctrl + z.
			std::cin.clear();
	} while (input.empty());

	return input;
}


/**
 * Read & Validate user's input according to main menu options.
 * If valid option, assign menuOption.
 */
bool ClientMenu::getMenuOption(MenuOption& menuOption) const
{
	const std::string input = readUserInput();
	const auto it = std::find_if(_menuOptions.begin(), _menuOptions.end(),
		[&input](auto& opt) { return (input == std::to_string(static_cast<uint32_t>(opt.getValue()))); });
	if (it == _menuOptions.end())
	{
		return false; // menuOption invalid.
	}
	menuOption = *it;
	return true;
}


/**
 * Invoke matching function to user's choice. User's choice is validated.
 */
void ClientMenu::handleUserChoice()
{
	MenuOption menuOption;
	bool success = getMenuOption(menuOption);
	while (!success)
	{
		std::cout << "Invalid input. Please try again.." << std::endl;
		success = getMenuOption(menuOption);
	}

	clear();
	std::cout << std::endl;
	if (!_registered && menuOption.requireRegistration())
	{
		std::cout << "You must register first!" << std::endl;
		return;
	}

	// Main selection switch
	switch (menuOption.getValue())
	{
	case MenuOption::EOption::MENU_EXIT:
	{
		std::cout << "Client will now exit." << std::endl;
		pause();
		exit(0);
	}
	case MenuOption::EOption::MENU_REGISTER:
	{
		if (_registered)
		{
			std::cout << _clientLogic.getSelfUsername() << ", you have already registered!" << std::endl;
			return;
		}
		const std::string username = readUserInput("Please type your username..");
		success = _clientLogic.registerClient(username);
		_registered = success;
		break;
	}
	//case MenuOption::EOption::MENU_SEND_PUBLIC_KEY:
	//{
	//	const std::string username = readUserInput("Please type a username..");
	//	success = _clientLogic.requestClientPublicKey(username);
	//	break;
	//}
	//case MenuOption::EOption::MENU_SEND_FILE:
	//{
	//	const std::string username = readUserInput("Please type a username to send file to..");
	//	const std::string message = readUserInput("Enter filepath: ");
	//	success = _clientLogic.sendMessage(username, MSG_FILE, message);
	//	break;
	//}
	}

	std::cout << (success ? menuOption.getSuccessString() : _clientLogic.getLastError()) << std::endl;
}
