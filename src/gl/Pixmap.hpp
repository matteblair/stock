//
// Created by Matt Blair on 12/18/16.
//
#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include "gl/GL.hpp"

namespace stock {

class Pixmap {

public:

  enum class PixelFormat : GLenum {
    ALPHA = GL_ALPHA,
    LUMINANCE = GL_LUMINANCE,
    LUMINANCE_ALPHA = GL_LUMINANCE_ALPHA,
    RGB = GL_RGB,
    RGBA = GL_RGBA,
  };

  enum class PixelType : GLenum {
    UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
  };

  // Construct an empty Pixmap.
  Pixmap();

  // Construct a Pixmap from a buffer of pixel data. The Pixmap takes ownership of the buffer.
  Pixmap(uint32_t width, uint32_t height, uint8_t* pixels, PixelFormat format, PixelType type = PixelType::UNSIGNED_BYTE);

  // Construct a Pixmap from a buffer of image file data (supports PNG, JPG, TGA, or PSD formats).
  // The Pixmap does not take ownership of the buffer.
  Pixmap(const uint8_t* rawFileData, size_t dataSize);

  // Construct a Pixmap from a buffer of image file data.
  Pixmap(const std::vector<uint8_t>& rawFileData) : Pixmap(rawFileData.data(), rawFileData.size()) {}

  // Destroy the Pixmap. This does not free any data owned by the Pixmap.
  ~Pixmap();

  // Free any data owned by this Pixmap.
  void dispose();

  // Get the pixel format of the Pixmap.
  PixelFormat format() const { return m_format; }

  // Get the pixel storage type of the Pixmap.
  PixelType type() const { return m_type; }

  // Get the pixel buffer of the Pixmap.
  uint8_t* pixels() const { return m_pixels; }

  // Get the dimensions of the Pixmap.
  uint32_t width() const { return m_width; }
  uint32_t height() const { return m_height; }

  // Get the number of color components in the Pixmap.
  uint32_t components() const;

protected:

  PixelFormat m_format = PixelFormat::RGBA;
  PixelType m_type = PixelType::UNSIGNED_BYTE;
  uint8_t* m_pixels = nullptr;
  uint32_t m_width = 0;
  uint32_t m_height = 0;
};

} // namespace stock
