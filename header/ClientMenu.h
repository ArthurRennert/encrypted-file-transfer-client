/**
 * Encrypted File Transfer Client
 * @file ClientMenu.h
 * @brief Interface class for user input. Handle user's requests.
 * @author Arthur Rennert
 */

#pragma once
#include "ClientLogic.h"
#include <string>       // std::to_string
#include <iomanip>      // std::setw

class ClientMenu
{
public:
	ClientMenu() : _registered(false), _rsaGenerated(false) {}
	void initialize();
	void display() const;
	void handleUserChoice();

	void clear() const { system("cls"); }     // clear menu
	void pause() const { system("pause"); }   // pause menu

private:

	class MenuOption
	{
	public:
		enum class EOption
		{
			MENU_REGISTER = 1,
			MENU_GENERATE_RSA_PAIR = 2,
			MENU_CHANGE_RSA_PAIR = 3,
			MENU_SEND_PUBLIC_KEY = 4,
			MENU_SEND_ENCRYPTED_FILE = 5,
			MENU_EXIT = 0
		};

	private:
		EOption     _value;
		bool        _registration;  // indicates whether registration is required before option usage.
		std::string _description;	
		std::string _success;       // success description

	public:
		MenuOption() : _value(EOption::MENU_EXIT), _registration(false) {}
		MenuOption(const EOption val, const bool reg, std::string desc, std::string success) : _value(val),
			_registration(reg), _description(std::move(desc)), _success(std::move(success)) {}

		friend std::ostream& operator<<(std::ostream& os, const MenuOption& opt) {
			os << std::setw(2) << static_cast<uint32_t>(opt._value) << ") " << opt._description;
			return os;
		}
		EOption getValue()             const { return _value; }
		bool requireRegistration()     const { return _registration; }
		std::string getDescription()   const { return _description; }
		std::string getSuccessString() const { return _success; }
	};

private:
	void clientStop(const std::string& error) const;
	std::string readUserInput(const std::string& description = "") const;
	bool getMenuOption(MenuOption& menuOption) const;


	ClientLogic                   _clientLogic;
	bool                          _registered;
	bool						  _rsaGenerated;
	const std::vector<MenuOption> _menuOptions{
		{ MenuOption::EOption::MENU_REGISTER,						false, "Register",                         "Successfully registered on server."},
		{ MenuOption::EOption::MENU_GENERATE_RSA_PAIR,				true,  "Generata RSA Pair",				   "RSA pair has been successfully generated."},
		{ MenuOption::EOption::MENU_CHANGE_RSA_PAIR,				true,  "Change RSA Pair",				   "RSA pair has been successfully changed."},
		{ MenuOption::EOption::MENU_SEND_PUBLIC_KEY,				true,  "Send public key",                  "Public key was sent successfully."},
		{ MenuOption::EOption::MENU_SEND_ENCRYPTED_FILE,            true,  "Send encrypted file",              "Encrypted file was sent successfully. CRC validated with Server."},
		{ MenuOption::EOption::MENU_EXIT,							false, "Exit client",                      ""}
	};
};

