/**
 * Encrypted File Transfer Client
 * @file Stringer.h
 * @brief Stringer class handles string manipulations using different libraries.
 * @author Arthur Rennert
 */

#pragma once
#include <string>

class Stringer
{
	static const std::string BASE64_CHARS;

public:
	static std::string encodeBase64(const std::string& str);
	static std::string decodeBase64(const std::string& str);

	static std::string hex(const uint8_t* buffer, const size_t size);
	static std::string unhex(const std::string& hexString);

	static void trim(std::string& stringToTrim);
};
