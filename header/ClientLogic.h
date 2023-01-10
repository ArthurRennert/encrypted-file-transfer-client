/**
 * Encrypted File Transfer Client
 * @file ClientLogic.h
 * @brief The core logic of Client.
 * ClientLogic received commands from ClientMenu and invokes internal logic such as FileHandler, SocketHandler.
 * @author Arthur Rennert
 */

#pragma once
#include "protocol.h"
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
		ClientID     id;
		std::string   username;
		PublicKey    publicKey;
		bool          publicKeySet = false;
		SymmetricKey symmetricKey;
		bool          symmetricKeySet = false;
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
	ClientID   getSelfClientID() const { return _self.id; }

	// client logic to be invoked by client menu.
	bool parseServeInfo();
	bool parseClientInfo();
	bool registerClient(const std::string& username);


private:
	void clearLastError();
	bool storeClientInfo();
	bool validateHeader(const ResponseHeader& header, const EResponseCode expectedCode);
	bool receiveUnknownPayload(const uint8_t* const request, const size_t reqSize, const EResponseCode expectedCode, uint8_t*& payload, size_t& size);
	//bool setClientSymmetricKey(const ClientID& clientID, const SymmetricKey& symmetricKey);

	Client              _self;           // self symmetric key invalid.
	std::stringstream    _lastError;
	FileHandler* _fileHandler;
	SocketHandler* _socketHandler;
	RSAPrivateWrapper* _rsaDecryptor;
};
