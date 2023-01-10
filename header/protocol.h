/**
 * Encrypted File Transfer Client
 * @file protocol.h
 * @brief Define protocol between client & server according to the requirements.
 * structs are aligned to 1 byte with the directive command pragma pack(push, 1).
 * @author Arthur Rennert
 */

#pragma once
#include <cstdint>

enum { DEFAULT_VALUE = 0 };  // Default value used to initialize protocol structures.

// Common types
typedef uint8_t  version_t;
typedef uint16_t code_t;
typedef uint8_t  messageType_t;
typedef uint32_t messageID_t;
typedef uint32_t csize_t;  // protocol's size type: Content's, payload's and message's size.

// Constants. All sizes are in BYTES.
constexpr version_t CLIENT_VERSION = 3;
constexpr size_t    CLIENT_ID_SIZE = 16;
constexpr size_t    CLIENT_NAME_SIZE = 255;
constexpr size_t    PUBLIC_KEY_SIZE = 160;  // defined in protocol. 1024 bits.
constexpr size_t    SYMMETRIC_KEY_SIZE = 16;   // defined in protocol.  128 bits.
constexpr size_t    REQUEST_OPTIONS = 5;
constexpr size_t    RESPONSE_OPTIONS = 6;

enum ERequestCode
{
	REQUEST_REGISTRATION = 1100,   // uuid ignored.
	REQUEST_PUBLIC_KEY = 1101,
	REQUEST_SEND_FILE = 1103,
	REQUEST_CRC_VALID = 1104,
	REQUEST_CRC_INVALID = 1005,
	REQUEST_CRC_INVALID_FOURTH_TIME = 1106
};

enum EResponseCode
{
	RESPONSE_REGISTRATION_SUCCESS = 2100,
	RESPONSE_REGISTRATION_FAILED = 2101,
	RESPONSE_AES_KEY = 2102,
	RESPONSE_SUCCESS_FILE_WITH_CRC = 2103,
	RESPONSE_MSG_RECEIVED_THANKS = 2104,
	RESPONSE_ERROR = 9999
};

enum EMessageType
{
	MSG_SYMMETRIC_KEY_REQUEST = 1,   // content invalid. contentSize = 0.
	MSG_SYMMETRIC_KEY_SEND = 2,   // content = symmetric key encrypted by destination client's public key.
	MSG_TEXT = 3,   // content = encrypted message by symmetric key.
	MSG_FILE = 4    // content = encrypted file by symmetric key.
};

#pragma pack(push, 1)

struct ClientID
{
	uint8_t uuid[CLIENT_ID_SIZE];
	ClientID() : uuid{ DEFAULT_VALUE } {}

	bool operator==(const ClientID& otherID) const {
		for (size_t i = 0; i < CLIENT_ID_SIZE; ++i)
			if (uuid[i] != otherID.uuid[i])
				return false;
		return true;
	}

	bool operator!=(const ClientID& otherID) const {
		return !(*this == otherID);
	}

};

struct ClientName
{
	uint8_t name[CLIENT_NAME_SIZE];  // DEFAULT_VALUE terminated.
	ClientName() : name{ '\0' } {}
};

struct PublicKey
{
	uint8_t publicKey[PUBLIC_KEY_SIZE];
	PublicKey() : publicKey{ DEFAULT_VALUE } {}
};

struct SymmetricKey
{
	uint8_t symmetricKey[SYMMETRIC_KEY_SIZE];
	SymmetricKey() : symmetricKey{ DEFAULT_VALUE } {}
};

struct RequestHeader
{
	ClientID       clientId;
	const version_t version;
	const code_t    code;
	csize_t         payloadSize;
	RequestHeader(const code_t reqCode) : version(CLIENT_VERSION), code(reqCode), payloadSize(DEFAULT_VALUE) {}
	RequestHeader(const ClientID& id, const code_t reqCode) : clientId(id), version(CLIENT_VERSION), code(reqCode), payloadSize(DEFAULT_VALUE) {}
};

struct ResponseHeader
{
	version_t version;
	code_t    code;
	csize_t   payloadSize;
	ResponseHeader() : version(DEFAULT_VALUE), code(DEFAULT_VALUE), payloadSize(DEFAULT_VALUE) {}
};

struct RequestRegistration
{
	RequestHeader header;
	struct
	{
		ClientName clientName;
		PublicKey  clientPublicKey;
	}payload;
	RequestRegistration() : header(REQUEST_REGISTRATION) {}
};

struct ResponseRegistration
{
	ResponseHeader header;
	ClientID       payload;
};


struct RequestPublicKey
{
	RequestHeader header;
	ClientID      payload;
	RequestPublicKey(const ClientID& id) : header(id, REQUEST_PUBLIC_KEY) {}
};

struct ResponsePublicKey
{
	ResponseHeader header;
	struct
	{
		ClientID   clientId;
		PublicKey  clientPublicKey;
	}payload;
};

#pragma pack(pop)