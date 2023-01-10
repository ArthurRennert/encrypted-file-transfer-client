/**
 * MessageU Client
 * @file Stringer.cpp
 * @brief Stringer class handles string manipulations using different libraries.
 * https://github.com/Romansko/MessageU/blob/main/client/src/Stringer.cpp
 */
#include "pch.h"
#include "Stringer.h"
#include <base64.h>
#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <chrono>

std::string Stringer::encodeBase64(const std::string& str)
{
	std::string encoded;
	CryptoPP::StringSource ss(str, true,
		new CryptoPP::Base64Encoder(
			new CryptoPP::StringSink(encoded)
		) // Base64Encoder
	); // StringSource

	return encoded;
}

std::string Stringer::decodeBase64(const std::string& str)
{
	std::string decoded;
	CryptoPP::StringSource ss(str, true,
		new CryptoPP::Base64Decoder(
			new CryptoPP::StringSink(decoded)
		) // Base64Decoder
	); // StringSource

	return decoded;
}


/**
 * Try to convert bytes to hex string representation.
 * Return empty string upon failure.
 */
std::string Stringer::hex(const uint8_t* buffer, const size_t size)
{
	if (size == 0 || buffer == nullptr)
		return "";
	const std::string byteString(buffer, buffer + size);
	if (byteString.empty())
		return "";
	try
	{
		return boost::algorithm::hex(byteString);
	}
	catch (...)
	{
		return "";
	}
}

/**
 * Try to convert hex string to bytes string.
 * Return empty string upon failure.
 */
std::string Stringer::unhex(const std::string& hexString)
{
	if (hexString.empty())
		return "";
	try
	{
		return boost::algorithm::unhex(hexString);
	}
	catch (...)
	{
		return "";
	}
}

/**
 * Trim a given string using boost algorithms library.
 */
void Stringer::trim(std::string& stringToTrim)
{
	boost::algorithm::trim(stringToTrim);
}

/**
 * Return current timestamp as sting.
 */
std::string Stringer::getTimestamp()
{
	const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	return std::to_string(now.count());
}

