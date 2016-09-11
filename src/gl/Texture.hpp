//
// Created by Matt Blair on 7/30/16.
//
#pragma once

#include "gl/GL.hpp"
#include <cstdint>

namespace stock {

class RenderState;

class Texture {

public:
  enum class PixelFormat : GLenum {
    ALPHA = GL_ALPHA,
    DEPTH_STENCIL = GL_DEPTH_STENCIL,
    DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
    LUMINANCE = GL_LUMINANCE,
    LUMINANCE_ALPHA = GL_LUMINANCE_ALPHA,
    RED = GL_RED,
    RED_INTEGER = GL_RED_INTEGER,
    RG = GL_RG,
    RGB = GL_RGB,
    RGB_INTEGER = GL_RGB_INTEGER,
    RGBA = GL_RGBA,
    RGBA_INTEGER = GL_RGBA_INTEGER,
  };

  enum class PixelType : GLenum {
    UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
    BYTE = GL_BYTE,
    UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
    SHORT = GL_SHORT,
    UNSIGNED_INT = GL_UNSIGNED_INT,
    INT = GL_INT,
    HALF_FLOAT = GL_HALF_FLOAT,
    FLOAT = GL_FLOAT,
    UNSIGNED_SHORT_5_6_5 = GL_UNSIGNED_SHORT_5_6_5,
    UNSIGNED_SHORT_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
    UNSIGNED_SHORT_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
    UNSIGNED_INT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV,
    UNSIGNED_INT_10F_11F_11F_REV = GL_UNSIGNED_INT_10F_11F_11F_REV,
    UNSIGNED_INT_5_9_9_9_REV = GL_UNSIGNED_INT_5_9_9_9_REV,
    UNSIGNED_INT_24_8 = GL_UNSIGNED_INT_24_8,
    FLOAT_32_UNSIGNED_INT_24_8_REV = GL_FLOAT_32_UNSIGNED_INT_24_8_REV,
  };

  enum class MinFilter : GLenum {
    NEAREST = GL_NEAREST,
    LINEAR = GL_LINEAR,
    NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
    LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
  };

  enum class MagFilter : GLenum {
    NEAREST = GL_NEAREST,
    LINEAR = GL_LINEAR,
  };

  enum class Wrap : GLenum {
    CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
    REPEAT = GL_REPEAT,
  };

  struct Options {
    PixelFormat pixelFormat = PixelFormat::RGBA;
    PixelType pixelType = PixelType::UNSIGNED_BYTE;
    MinFilter minFilter = MinFilter::LINEAR;
    MagFilter magFilter = MagFilter::LINEAR;
    Wrap wrapS = Wrap::CLAMP_TO_EDGE;
    Wrap wrapT = Wrap::CLAMP_TO_EDGE;
    bool generateMipmaps = false;
  };

  // Create an empty Texture with default options.
  Texture();

  // Destroy the Texture and free the pixel data.
  ~Texture();

  // Create a Texture from a buffer of pixel data; the Texture takes ownership
  // of the buffer.
  Texture(uint32_t width, uint32_t height, uint8_t* data, size_t size, Options options);

  // Load pixel data from the contents of an image file (JPG, PNG, GIF, BMP);
  // this must be done before the first time
  // that prepare() or bind() is called on the Texture; the Texture does NOT
  // take ownership of the buffer.
  static bool loadImageFileData(Texture* texture, const uint8_t* data, size_t size, Options options);

  // Get the image dimensions of the Texture.
  uint32_t width() const;
  uint32_t height() const;

  // Get the OpenGL handle of the Texture.
  GLuint glHandle() const;

  // Initialize OpenGL resources for the Texture, if needed.
  void prepare(RenderState& rs);

  // Bind the Texture to the current texture unit.
  void bind(RenderState& rs);

  // Delete any OpenGL resources for the Texture.
  void dispose(RenderState& rs);

protected:
  Options m_options;

  // Generation in which this texture's GL handle was created
  int m_generation = -1;

  // Image dimensions in pixels
  uint32_t m_width = 0;
  uint32_t m_height = 0;

  // OpenGL texture identifiers
  GLuint m_glHandle = 0;
  GLenum m_target = GL_TEXTURE_2D;

  // Source pixel data
  static uint8_t defaultPixelData[];
  uint8_t* m_data = defaultPixelData;
  size_t m_size = 0;
};

} // namespace stock
