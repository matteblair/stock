//
// Created by Matt Blair on 12/18/16.
//

#include "Pixmap.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "stb_image.h"

namespace stock {

Pixmap::Pixmap() {}

Pixmap::Pixmap(uint32_t width, uint32_t height, uint8_t* pixels, PixelFormat format, PixelType type)
    : m_format(format), m_type(type), m_pixels(pixels), m_width(width), m_height(height) {}

Pixmap::Pixmap(const uint8_t* data, size_t dataSize) {

  // stbi_load_from_memory loads the image as starting from the top-left corner. This call flips the output such that
  // the data begins at the bottom-left corner, as required for OpenGL texture data.
  stbi_set_flip_vertically_on_load(1);

  int width = 0, height = 0, components = 0, size = static_cast<int>(dataSize);
  uint8_t* pixels = nullptr;

  if (data != nullptr && size != 0) {
    pixels = stbi_load_from_memory(data, size, &width, &height, &components, 0);
  }

  if (pixels != nullptr) {
    m_pixels = pixels;
    m_width = static_cast<uint32_t>(width);
    m_height = static_cast<uint32_t>(height);

    switch (components) {
    case 1: m_format = PixelFormat::ALPHA; break;
    case 2: m_format = PixelFormat::LUMINANCE_ALPHA; break;
    case 3: m_format = PixelFormat::RGB; break;
    case 4: m_format = PixelFormat::RGBA; break;
    default: break;
    }
  }
}

Pixmap::~Pixmap() {}

void Pixmap::dispose() {
  free(m_pixels);
  m_pixels = nullptr;
}

uint32_t Pixmap::components() const {
  switch (m_format) {
  case PixelFormat::ALPHA: return 1;
  case PixelFormat::LUMINANCE: return 1;
  case PixelFormat::LUMINANCE_ALPHA: return 2;
  case PixelFormat::RGB: return 3;
  case PixelFormat::RGBA: return 4;
  }
}

} // namespace stock
