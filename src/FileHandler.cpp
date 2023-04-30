/**
 * Encrypted File Transfer Client
 * @file FileHandler.cpp
 * @brief Handle files on filesystem.
 * @author Arthur Rennert
 */

#include "pch.h"
#include "FileHandler.h"
#include <algorithm>
#include <fstream>
#include <boost/filesystem.hpp>  // for create_directories


FileHandler::FileHandler() : _fileStream(nullptr), _open(false)
{
}

FileHandler::~FileHandler()
{
	close();
}

/**
 * Open a file for read/write. Create folders in filepath if do not exist.
 * Relative paths not supported!
 */
bool FileHandler::open(const std::string& filepath, bool write)
{
	const auto flags = write ? (std::fstream::binary | std::fstream::out) : (std::fstream::binary | std::fstream::in);
	if (filepath.empty())
		return false;

	try
	{
		close(); // close and clear current fstream before allocating new one.
		_fileStream = new std::fstream;
		// create directories within the path if they are do not exist.
		const auto parent = boost::filesystem::path(filepath).parent_path();
		if (!parent.empty())
		{
			(void)create_directories(parent);
		}
		_fileStream->open(filepath, flags);
		_open = _fileStream->is_open();
	}
	catch (...)
	{
		_open = false;
	}
	return _open;
}


/**
 * Open a file for write (append). Create folders in filepath if do not exist.
 * Relative paths not supported!
 */
bool FileHandler::openToAppend(const std::string& filepath)
{
	if (filepath.empty())
		return false;

	try
	{
		close(); // close and clear current fstream before allocating new one.
		_fileStream = new std::fstream;
		// create directories within the path if they are do not exist.
		const auto parent = boost::filesystem::path(filepath).parent_path();
		if (!parent.empty())
		{
			(void)create_directories(parent);
		}
		_fileStream->open(filepath, std::ios::out | std::ios::app);
		_open = _fileStream->is_open();
	}
	catch (...)
	{
		_open = false;
	}
	return _open;
}

/**
 * Close file stream.
 */
void FileHandler::close()
{
	try
	{
		if (_fileStream != nullptr)
			_fileStream->close();
	}
	catch (...)
	{
		/* Do Nothing */
	}
	delete _fileStream;
	_fileStream = nullptr;
	_open = false;
}

/**
 * Read bytes from fs to dest.
 */
bool FileHandler::read(uint8_t* const dest, const size_t bytes) const
{
	if (_fileStream == nullptr || !_open || dest == nullptr || bytes == 0)
		return false;
	try
	{
		_fileStream->read(reinterpret_cast<char*>(dest), bytes);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

/**
 * Write given bytes from src to fs.
 */
bool FileHandler::write(const uint8_t* const src, const size_t bytes) const
{
	if (_fileStream == nullptr || !_open || src == nullptr || bytes == 0)
		return false;
	try
	{
		_fileStream->write(reinterpret_cast<const char*>(src), bytes);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

/**
 * Read a single line from fs to line.
 */
bool FileHandler::readLine(std::string& line) const
{
	if (_fileStream == nullptr || !_open)
		return false;
	try
	{
		if (!std::getline(*_fileStream, line) || line.empty())
			return false;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

/**
 * Write a single line and append an end line character.
 */
bool FileHandler::writeLine(const std::string& line) const
{
	std::string newline = line;
	newline.append("\n");
	return write(reinterpret_cast<const uint8_t*>(newline.c_str()), newline.size());  // write without null termination.
}

/**
 * Calculate the file size which is opened by fs.
 */
size_t FileHandler::size() const
{
	if (_fileStream == nullptr || !_open)
		return 0;
	try
	{
		const auto cur = _fileStream->tellg();
		_fileStream->seekg(0, std::fstream::end);
		const auto size = _fileStream->tellg();
		if ((size <= 0) || (size > UINT32_MAX))    // do not support more than uint32 max size files. (up to 4GB).
			return 0;
		_fileStream->seekg(cur);    // restore position
		return static_cast<size_t>(size);
	}
	catch (...)
	{
		return 0;
	}
}

/**
 * Open and read file.
 * Caller is responsible for freeing allocated memory upon success.
 */
bool FileHandler::readAtOnce(const std::string& filepath, uint8_t*& file, size_t& bytes)
{
	if (!open(filepath))
		return false;

	bytes = size();
	if (bytes == 0)
		return false;

	file = new uint8_t[bytes];
	const bool success = read(file, bytes);
	if (!success)
	{
		delete[] file;
	}
	close();
	return success;
}
