/**
 * Encrypted File Transfer Client
 * @file SocketHandler.h
 * @brief Handle sending and receiving from a socket.
 * @author Arthur Rennert
 */

#pragma once
#include <string>
#include <cstdint>
#include <ostream>
#include <boost/asio/ip/tcp.hpp>

using boost::asio::ip::tcp;
using boost::asio::io_context;

constexpr size_t PACKET_SIZE = 1024;   // The same on server side.

class SocketHandler
{
public:
	SocketHandler();
	virtual ~SocketHandler();

	// do not allow
	SocketHandler(const SocketHandler& other) = delete;
	SocketHandler(SocketHandler&& other) noexcept = delete;
	SocketHandler& operator=(const SocketHandler& other) = delete;
	SocketHandler& operator=(SocketHandler&& other) noexcept = delete;

	friend std::ostream& operator<<(std::ostream& os, const SocketHandler* socket) {
		if (socket != nullptr)
			os << socket->_address << ':' << socket->_port;
		return os;
	}
	friend std::ostream& operator<<(std::ostream& os, const SocketHandler& socket) {
		return operator<<(os, &socket);
	}

	// validations
	static bool isValidAddress(const std::string& address);
	static bool isValidPort(const std::string& port);

	// logic
	bool setSocketInfo(const std::string& address, const std::string& port);
	bool connect();
	void close();
	bool receive(uint8_t* const buffer, const size_t size) const;
	bool send(const uint8_t* const buffer, const size_t size) const;
	bool sendReceive(const uint8_t* const toSend, const size_t size, uint8_t* const response, const size_t resSize);
	bool sendOnly(const uint8_t* const toSend, const size_t size);

private:
	std::string     _address;
	std::string     _port;
	io_context*		_ioContext;
	tcp::resolver*  _resolver;
	tcp::socket*	_socket;
	bool		    _bigEndian;
	bool            _connected;  // indicates that socket is open and connected.

	void swapBytes(uint8_t* const buffer, size_t size) const;
};
