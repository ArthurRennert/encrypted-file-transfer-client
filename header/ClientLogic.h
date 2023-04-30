/**
 * Encrypted File Transfer Client
 * @file ClientLogic.h
 * @brief The core logic of Client.
 * ClientLogic received commands from ClientMenu and invokes internal logic such as FileHandler, SocketHandler.
 * @author Arthur Rennert
 */

#pragma once
#include "protocol.h"
#include <boost/crc.hpp>
#include <sstream>
#include <string>
#include <vector>

constexpr auto CLIENT_INFO = "me.info";   // Should be located near exe file.
constexpr auto SERVER_INFO = "transfer.info";  // Should be located near exe file.

class FileHandler;
class SocketHandler;
class RSAPrivateWrapper;

class ClientLogic
{
public:

	struct Client
	{
		ClientID		id;
		std::string		username;
		PublicKey		publicKey;
		bool			publicKeySet = false;
		AESKey			symmetricKey;
		bool			symmetricKeySet = false;
		bool			validCRC = false;
	};


public:
	ClientLogic();
	virtual ~ClientLogic();
	ClientLogic(const ClientLogic& other) = delete;
	ClientLogic(ClientLogic&& other) noexcept = delete;
	ClientLogic& operator=(const ClientLogic& other) = delete;
	ClientLogic& operator=(ClientLogic&& other) noexcept = delete;

	// inline getters
	std::string getLastError() const { return _lastError.str(); }
	std::string getSelfUsername() const { return _self.username; }

	// client logic to be invoked by client menu.
	bool parseServeInfo();
	bool parseFileName(std::string& fileName);
	bool parseRegisteredClientInfo();
	bool parseUnregisteredClientInfo(std::string& username);
	bool registerClient(const std::string& username);
	bool generateRSAPair();
	bool changeRSAPair();
	bool sendPublicKey();
	bool sendFile(bool& sent);

	uint32_t getCRC(const std::string& str);

	bool isRSAGenerated();
	bool isSymmetricKeySet();
	bool informServerCRCValidated(uint8_t* fileName);
	bool isCRCValid();
	void informServerCRCFailed(const size_t size);

private:
	void clearLastError();
	bool storeClientInfo();
	bool storeClientRSA();
	bool validateHeader(const ResponseHeader& header, const ResponseCode expectedCode);

	Client              _self;           
	std::stringstream    _lastError;
	FileHandler* _fileHandler;
	SocketHandler* _socketHandler;
	RSAPrivateWrapper* _rsaDecryptor;
};
