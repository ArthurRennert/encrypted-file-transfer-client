/**
 * Encrypted File Transfer Client
 * @file AESWrapper.h
 * @brief Handle symmetric encryption.
 * @author Arthur Rennert
 */

#pragma once
#include <string>
#include "protocol.h"

class AESWrapper
{
public:
	static void GenerateKey(uint8_t* const buffer, const size_t length);

	AESWrapper();
	AESWrapper(const AESKey& symKey);

	virtual ~AESWrapper() = default;
	AESWrapper(const AESWrapper& other) = delete;
	AESWrapper(AESWrapper&& other) noexcept = delete;
	AESWrapper& operator=(const AESWrapper& other) = delete;
	AESWrapper& operator=(AESWrapper&& other) noexcept = delete;

	AESKey getKey() const { return _key; }

	std::string encrypt(const uint8_t* plain, size_t length) const;

private:
	AESKey _key;
};
