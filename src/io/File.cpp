//
// Created by Matt Blair on 7/31/16.
//

#include "io/File.hpp"

namespace stock {

File::File(const std::string& path) : m_path(path) {}

File::~File() { close(); }

bool File::open() {

  if (!m_isOpen) {
    m_handle = std::fopen(m_path.c_str(), "rb");
    return m_handle != nullptr;
  }

  return false;
}

bool File::close() {

  if (m_isOpen) {
    return std::fclose(m_handle) == 0;
  }

  return false;
}

bool File::exists() {

  open();
  return m_handle != nullptr;
}

size_t File::getSize() {

  if (!exists()) {
    return 0;
  }

  std::fseek(m_handle, 0, SEEK_END);
  size_t size = std::ftell(m_handle);
  std::fseek(m_handle, 0, SEEK_SET);

  return size;
}

size_t File::read(uint8_t* buffer, size_t start, size_t count) {

  if (!exists()) {
    return 0;
  }

  std::fseek(m_handle, start, SEEK_SET);
  size_t read = std::fread(buffer, 1, count, m_handle);
  std::fseek(m_handle, 0, SEEK_SET);

  return read;
}

std::vector<uint8_t> File::readAll() {

  auto size = getSize();
  std::vector<uint8_t> result(size, 0);
  read(result.data(), 0, size);

  return result;
}

} // namespace stock
