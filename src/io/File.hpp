//
// Created by Matt Blair on 7/31/16.
//

#pragma once

#include <cstdio>
#include <string>
#include <vector>

namespace stock {

class File {

public:

    // Create a File representing the given path.
    File(const std::string& path);

    // Destroy the File object and close any open path.
    ~File();

    // Open the path for this File; returns true if opening was successful, returns false if opening was unsuccessful or
    // if the File was already opened.
    bool open();

    // Close the path for this File; returns true if closing was successful, returns false if closing was unsuccessful
    // or if the File was not opened.
    bool close();

    // Get whether this File object represents an existing path in the filesystem; if the File is not open this will
    // attempt to open it.
    bool exists();

    // Get the size of this File in bytes; if the File is not open this will attempt to open it; returns 0 if the File
    // does not exist.
    size_t getSize();

    // Starting at 'start' bytes into the File, read 'count' bytes of the File and copy them into 'buffer'; if the File
    // is not open this will attempt to open it; returns the number of bytes successfully read.
    size_t read(uint8_t* buffer, size_t start, size_t count);

    // Read the entire contents of the file into a vector of bytes; if the File is not open this will attempt to open
    // it; if the File does not exist the returned vector is empty.
    std::vector<uint8_t> readAll();

protected:

    std::string m_path;
    std::FILE* m_handle = nullptr;
    bool m_isOpen = false;

};

} // namespace stock
