/**
 * Encrypted File Transfer Client
 * @file ClientLogic.cpp
 * @brief The core logic of Client.
 * ClientLogic received commands from ClientMenu and invokes internal logic such as FileHandler, SocketHandler.
 * @author Arthur Rennert
 */

#include "pch.h"
#include "ClientLogic.h"
#include "Stringer.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include "FileHandler.h"
#include "SocketHandler.h"

std::ostream& operator<<(std::ostream& os, const EMessageType& type)
{
	os << static_cast<messageType_t>(type);
	return os;
}

ClientLogic::ClientLogic() : _fileHandler(nullptr), _socketHandler(nullptr), _rsaDecryptor(nullptr)
{
	_fileHandler = new FileHandler();
	_socketHandler = new SocketHandler();
}

ClientLogic::~ClientLogic()
{
	delete _fileHandler;
	delete _socketHandler;
	delete _rsaDecryptor;
}

/**
 * Parse SERVER_INFO file for server address & port.
 */
bool ClientLogic::parseServeInfo()
{
	std::stringstream err;
	if (!_fileHandler->open(SERVER_INFO))
	{
		clearLastError();
		_lastError << "Couldn't open " << SERVER_INFO;
		return false;
	}
	std::string info;
	if (!_fileHandler->readLine(info))
	{
		clearLastError();
		_lastError << "Couldn't read " << SERVER_INFO;
		return false;
	}
	_fileHandler->close();
	Stringer::trim(info);
	const auto pos = info.find(':');
	if (pos == std::string::npos)
	{
		clearLastError();
		_lastError << SERVER_INFO << " has invalid format! missing separator ':'";
		return false;
	}
	const auto address = info.substr(0, pos);
	const auto port = info.substr(pos + 1);
	if (!_socketHandler->setSocketInfo(address, port))
	{
		clearLastError();
		_lastError << SERVER_INFO << " has invalid IP address or port!";
		return false;
	}
	return true;
}

/**
 * Parse CLIENT_INFO file.
 */
bool ClientLogic::parseClientInfo()
{
	std::string line;
	if (!_fileHandler->open(CLIENT_INFO))
	{
		clearLastError();
		_lastError << "Couldn't open " << CLIENT_INFO;
		return false;
	}

	// Read & Parse username
	if (!_fileHandler->readLine(line))
	{
		clearLastError();
		_lastError << "Couldn't read username from " << CLIENT_INFO;
		return false;
	}
	Stringer::trim(line);
	if (line.length() >= CLIENT_NAME_SIZE)
	{
		clearLastError();
		_lastError << "Invalid username read from " << CLIENT_INFO;
		return false;
	}
	_self.username = line;

	// Read & Parse Client's UUID.
	if (!_fileHandler->readLine(line))
	{
		clearLastError();
		_lastError << "Couldn't read client's UUID from " << CLIENT_INFO;
		return false;
	}

	line = Stringer::unhex(line);
	const char* unhexed = line.c_str();
	if (strlen(unhexed) != sizeof(_self.id.uuid))
	{
		memset(_self.id.uuid, 0, sizeof(_self.id.uuid));
		clearLastError();
		_lastError << "Couldn't parse client's UUID from " << CLIENT_INFO;
		return false;
	}
	memcpy(_self.id.uuid, unhexed, sizeof(_self.id.uuid));

	// Read & Parse Client's private key.
	std::string decodedKey;
	while (_fileHandler->readLine(line))
	{
		decodedKey.append(Stringer::decodeBase64(line));
	}
	if (decodedKey.empty())
	{
		clearLastError();
		_lastError << "Couldn't read client's private key from " << CLIENT_INFO;
		return false;
	}
	try
	{
		delete _rsaDecryptor;
		_rsaDecryptor = new RSAPrivateWrapper(decodedKey);
	}
	catch (...)
	{
		clearLastError();
		_lastError << "Couldn't parse private key from " << CLIENT_INFO;
		return false;
	}
	_fileHandler->close();
	return true;
}


/**
 * Reset _lastError StringStream: Empty string, clear errors flag and reset formatting.
 */
void ClientLogic::clearLastError()
{
	const std::stringstream clean;
	_lastError.str("");
	_lastError.clear();
	_lastError.copyfmt(clean);
}

/**
 * Store client info to CLIENT_INFO file.
 */
bool ClientLogic::storeClientInfo()
{
	if (!_fileHandler->open(CLIENT_INFO, true))
	{
		clearLastError();
		_lastError << "Couldn't open " << CLIENT_INFO;
		return false;
	}

	// Write username
	if (!_fileHandler->writeLine(_self.username))
	{
		clearLastError();
		_lastError << "Couldn't write username to " << CLIENT_INFO;
		return false;
	}

	// Write UUID.
	const auto hexifiedUUID = Stringer::hex(_self.id.uuid, sizeof(_self.id.uuid));
	if (!_fileHandler->writeLine(hexifiedUUID))
	{
		clearLastError();
		_lastError << "Couldn't write UUID to " << CLIENT_INFO;
		return false;
	}

	// Write Base64 encoded private key
	const auto encodedKey = Stringer::encodeBase64(_rsaDecryptor->getPrivateKey());
	if (!_fileHandler->write(reinterpret_cast<const uint8_t*>(encodedKey.c_str()), encodedKey.size()))
	{
		clearLastError();
		_lastError << "Couldn't write client's private key to " << CLIENT_INFO;
		return false;
	}

	_fileHandler->close();
	return true;
}

/**
 * Validate ResponseHeader upon an expected EResponseCode.
 */
bool ClientLogic::validateHeader(const ResponseHeader& header, const EResponseCode expectedCode)
{
	if (header.code == RESPONSE_ERROR)
	{
		clearLastError();
		_lastError << "Generic error response code (" << RESPONSE_ERROR << ") received.";
		return false;
	}

	if (header.code != expectedCode)
	{
		clearLastError();
		_lastError << "Unexpected response code " << header.code << " received. Expected code was " << expectedCode;
		return false;
	}

	csize_t expectedSize = DEFAULT_VALUE;
	switch (header.code)
	{
	case RESPONSE_REGISTRATION_SUCCESS:
	{
		expectedSize = sizeof(ResponseRegistration) - sizeof(ResponseHeader);
		break;
	}
	//case RESPONSE_REGISTRATION_FAILED:
	//{

	//	break;
	//}
	//case RESPONSE_AES_KEY:
	//{
	//
	//	break;
	//}
	//case RESPONSE_SUCCESS_FILE_WITH_CRC:
	//{
	//
	//	break;
	//}
	//case RESPONSE_MSG_RECEIVED_THANKS:
	//{
	//
	//	break;
	//}
	//case RESPONSE_ERROR:
	//{
	//
	//	break;
	//}
	default:
	{
		return true;  // variable payload size. 
	}
	}

	if (header.payloadSize != expectedSize)
	{
		clearLastError();
		_lastError << "Unexpected payload size " << header.payloadSize << ". Expected size was " << expectedSize;
		return false;
	}

	return true;
}

/**
 * Receive unknown payload. Payload size is parsed from header.
 * Caller responsible for deleting payload upon success.
 */
bool ClientLogic::receiveUnknownPayload(const uint8_t* const request, const size_t reqSize, const EResponseCode expectedCode, uint8_t*& payload, size_t& size)
{
	ResponseHeader response;
	uint8_t buffer[PACKET_SIZE];
	payload = nullptr;
	size = 0;
	if (request == nullptr || reqSize == 0)
	{
		clearLastError();
		_lastError << "Invalid request was provided";
		return false;
	}
	if (!_socketHandler->connect())
	{
		clearLastError();
		_lastError << "Failed connecting to server on " << _socketHandler;
		return false;
	}
	if (!_socketHandler->send(request, reqSize))
	{
		_socketHandler->close();
		clearLastError();
		_lastError << "Failed sending request to server on " << _socketHandler;
		return false;
	}
	if (!_socketHandler->receive(buffer, sizeof(buffer)))
	{
		clearLastError();
		_lastError << "Failed receiving response header from server on " << _socketHandler;
		return false;
	}
	memcpy(&response, buffer, sizeof(ResponseHeader));
	if (!validateHeader(response, expectedCode))
	{
		clearLastError();
		_lastError << "Received unexpected response code from server on  " << _socketHandler;
		return false;
	}
	if (response.payloadSize == 0)
		return true;  // no payload. but not an error.

	size = response.payloadSize;
	payload = new uint8_t[size];
	uint8_t* ptr = static_cast<uint8_t*>(buffer) + sizeof(ResponseHeader);
	size_t recSize = sizeof(buffer) - sizeof(ResponseHeader);
	if (recSize > size)
		recSize = size;
	memcpy(payload, ptr, recSize);
	ptr = payload + recSize;
	while (recSize < size)
	{
		size_t toRead = (size - recSize);
		if (toRead > PACKET_SIZE)
			toRead = PACKET_SIZE;
		if (!_socketHandler->receive(buffer, toRead))
		{
			clearLastError();
			_lastError << "Failed receiving payload data from server on " << _socketHandler;
			delete[] payload;
			payload = nullptr;
			size = 0;
			return false;
		}
		memcpy(ptr, buffer, toRead);
		recSize += toRead;
		ptr += toRead;
	}

	return true;
}


///**
// * Store a client's symmetric key on RAM.
// */
//bool ClientLogic::setClientSymmetricKey(const ClientID& clientID, const SymmetricKey& symmetricKey)
//{
//	for (Client& client : _clients)
//	{
//		if (client.id == clientID)
//		{
//			client.symmetricKey = symmetricKey;
//			client.symmetricKeySet = true;
//			return true;
//		}
//	}
//	return false;
//}


/**
 * Register client via the server.
 */
bool ClientLogic::registerClient(const std::string& username)
{
	RequestRegistration  request;
	ResponseRegistration response;

	if (username.length() >= CLIENT_NAME_SIZE)  // >= because of null termination.
	{
		clearLastError();
		_lastError << "Invalid username length!";
		return false;
	}
	for (auto ch : username)
	{
		if (!std::isalnum(ch))  // check that username is alphanumeric. [a-zA-Z0-9].
		{
			clearLastError();
			_lastError << "Invalid username! Username may only contain letters and numbers!";
			return false;
		}
	}

	delete _rsaDecryptor;
	_rsaDecryptor = new RSAPrivateWrapper();
	const auto publicKey = _rsaDecryptor->getPublicKey();
	if (publicKey.size() != PUBLIC_KEY_SIZE)
	{
		clearLastError();
		_lastError << "Invalid public key length!";
		return false;
	}

	// fill request data
	request.header.payloadSize = sizeof(request.payload);
	strcpy_s(reinterpret_cast<char*>(request.payload.clientName.name), CLIENT_NAME_SIZE, username.c_str());
	memcpy(request.payload.clientPublicKey.publicKey, publicKey.c_str(), sizeof(request.payload.clientPublicKey.publicKey));

	if (!_socketHandler->sendReceive(reinterpret_cast<const uint8_t* const>(&request), sizeof(request),
		reinterpret_cast<uint8_t* const>(&response), sizeof(response)))
	{
		clearLastError();
		_lastError << "Failed communicating with server on " << _socketHandler;
		return false;
	}

	// parse and validate ResponseRegistration
	if (!validateHeader(response.header, RESPONSE_REGISTRATION_SUCCESS))
		return false;  // error message updated within.

	// store received client's ID
	_self.id = response.payload;
	_self.username = username;
	_self.publicKey = request.payload.clientPublicKey;
	if (!storeClientInfo())
	{
		clearLastError();
		_lastError << "Failed writing client info to " << CLIENT_INFO << ". Please register again with different username.";
		return false;
	}

	return true;
}
