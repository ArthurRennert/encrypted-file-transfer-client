/**
 * Encrypted File Transfer Client
 * @file FileHandler.h
 * @brief Handle files on filesystem.
 * @author Arthur Rennert
 */

#pragma once
#include <string>
#include <fstream>

class FileHandler
{
public:
    FileHandler();
    virtual ~FileHandler();

    // do not allow
    FileHandler(const FileHandler& other) = delete;
    FileHandler(FileHandler&& other) noexcept = delete;
    FileHandler& operator=(const FileHandler& other) = delete;
    FileHandler& operator=(FileHandler&& other) noexcept = delete;

    // file wrapper functions
    bool open(const std::string& filepath, bool write = false);
    bool openToAppend(const std::string& filepath);
    void close();
    bool read(uint8_t* const dest, const size_t bytes) const;
    bool write(const uint8_t* const src, const size_t bytes) const;
    bool readLine(std::string& line) const;
    bool writeLine(const std::string& line) const;
    size_t size() const;

    bool readAtOnce(const std::string& filepath, uint8_t*& file, size_t& bytes);

private:
    std::fstream* _fileStream;
    bool          _open;  // indicates whether a file is open.
};
