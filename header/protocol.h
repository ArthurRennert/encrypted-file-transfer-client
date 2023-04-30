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
typedef uint8_t version_t;
typedef uint16_t code_t;
typedef uint32_t csize_t;  // protocol's size type: Content's, payload's and message's size.

// Constants. All sizes are in BYTES.
constexpr version_t CLIENT_VERSION = 3;
constexpr size_t    CLIENT_ID_SIZE = 16;
constexpr size_t    CLIENT_NAME_SIZE = 255;
constexpr size_t    CONTENT_SIZE = 4;	//file's size (after encryption)
constexpr size_t    FILE_NAME_SIZE = 255;
constexpr size_t    PUBLIC_KEY_SIZE = 160;  // defined in protocol. 1024 bits.
constexpr size_t    AES_KEY_SIZE = 16;   // defined in protocol.  128 bits.
constexpr size_t    ENCRYPTED_AES_KEY_SIZE = 128; 
constexpr size_t    REQUEST_OPTIONS = 6;
constexpr size_t    RESPONSE_OPTIONS = 6;
constexpr size_t    MAX_FILE_RESEND_RETRIES = 3;

enum RequestCode
{
	REQUEST_REGISTRATION = 1100,   // uuid ignored.
	REQUEST_SEND_PUBLIC_KEY = 1101,
	REQUEST_SEND_FILE = 1103,
	REQUEST_SEND_VALID_CRC = 1104,
	REQUEST_INVALID_CRC = 1005,
	REQUEST_INVALID_CRC_FOURTH_TIME = 1106
};

enum ResponseCode
{
	RESPONSE_REGISTRATION_SUCCESS = 2100,
	RESPONSE_REGISTRATION_FAILED = 2101,
	RESPONSE_ENCRYPTED_AES_KEY = 2102,
	RESPONSE_SUCCESS_FILE_WITH_CRC = 2103,
	RESPONSE_MSG_RECEIVED_THANKS = 2104,
	RESPONSE_ERROR = 9999
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
	uint8_t name[CLIENT_NAME_SIZE];
	ClientName() : name{ '\0' } {}
};

struct PublicKey
{
	uint8_t publicKey[PUBLIC_KEY_SIZE];
	PublicKey() : publicKey{ DEFAULT_VALUE } {}
};

struct File
{
	uint8_t fileName[FILE_NAME_SIZE];
	File() : fileName{ '\0' } {}
};

struct AESKey
{
	uint8_t symmetricKey[AES_KEY_SIZE];
	AESKey() : symmetricKey{ DEFAULT_VALUE } {}
};

struct EncryptedAESKey
{
	uint8_t encryptedAESKey[ENCRYPTED_AES_KEY_SIZE];
	EncryptedAESKey() : encryptedAESKey{ DEFAULT_VALUE } {}
};

struct RequestHeader
{
	ClientID		clientId;
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
	ClientName clientName;

	RequestRegistration() : header(REQUEST_REGISTRATION) {}
};

struct ResponseRegistrationSucceed
{
	ResponseHeader	header;
	ClientID		payload;
};

struct ResponseRegistrationFailed
{
	ResponseHeader	header;
};

struct RequestSendPublicKey
{
	RequestHeader header;
	struct
	{
		ClientName   clientName;
		PublicKey	 clientPublicKey;
	}payload;
	RequestSendPublicKey() : header(REQUEST_SEND_PUBLIC_KEY) {}
};

struct ResponseEncryptedKey
{
	ResponseHeader header;
	struct
	{
		ClientID		  clientId;
		EncryptedAESKey   encryptedAESKey;
	}payload;
};

struct RequestSendFile
{
	RequestHeader header;
	struct PayloadHeader
	{
		csize_t     contentSize;
		File		file;
		PayloadHeader() : contentSize(DEFAULT_VALUE) {}
	}PayloadHeader;

	RequestSendFile(const ClientID& id) : header(id, REQUEST_SEND_FILE) {}
};

struct ResponseFileAcception
{
	ResponseHeader header;
	struct PayloadHeader
	{
		ClientID       clientId;
		csize_t        contentSize;
		File		   file;
		csize_t		   crc;
		PayloadHeader() : contentSize(DEFAULT_VALUE), crc(DEFAULT_VALUE) {}
	}PayloadHeader;
};

struct RequestValidCRC
{
	RequestHeader	header;
	File			file;
	RequestValidCRC(const ClientID& id) : header(id, REQUEST_SEND_VALID_CRC) {}
};

struct ResponseMSGReceived
{
	ResponseHeader  header;
	ClientID		clientId;
};

struct RequestInvalidCRC
{
	RequestHeader header;
	RequestInvalidCRC(const ClientID& id) : header(id, REQUEST_INVALID_CRC) {}
};

struct RequestInvalidCRCAbort
{
	RequestHeader header;
	RequestInvalidCRCAbort(const ClientID& id) : header(id, REQUEST_INVALID_CRC_FOURTH_TIME) {}
};

#pragma pack(pop)