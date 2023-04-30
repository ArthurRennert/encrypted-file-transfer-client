/**
 * Encrypted File Transfer Client
 * @file RSAWrapper.cpp
 * @brief Handle asymmetric encryption.
 * @author Arthur Rennert
 */

#include "pch.h"
#include "RSAWrapper.h"
#include "protocol.h"


RSAPublicWrapper::RSAPublicWrapper(const PublicKey& publicKey)
{
	CryptoPP::StringSource ss((publicKey.publicKey), sizeof(publicKey.publicKey), true);
	_publicKey.Load(ss);
}

RSAPrivateWrapper::RSAPrivateWrapper()
{
	_privateKey.Initialize(_rng, BITS);
}

RSAPrivateWrapper::RSAPrivateWrapper(const std::string& key)
{
	CryptoPP::StringSource ss(key, true);
	_privateKey.Load(ss);
}

std::string RSAPrivateWrapper::getPrivateKey() const
{
	std::string key;
	CryptoPP::StringSink ss(key);
	_privateKey.Save(ss);
	return key;
}

std::string RSAPrivateWrapper::getPublicKey() const
{
	const CryptoPP::RSAFunction publicKey((_privateKey));
	std::string key;
	CryptoPP::StringSink ss(key);
	publicKey.Save(ss);
	return key;
}

std::string RSAPrivateWrapper::decrypt(const uint8_t* cipher, size_t length)
{
	std::string decrypted;
	CryptoPP::RSAES_OAEP_SHA_Decryptor d(_privateKey);
	CryptoPP::StringSource ss_cipher((cipher), length, true, new CryptoPP::PK_DecryptorFilter(_rng, d, new CryptoPP::StringSink(decrypted)));
	return decrypted;
}
