//
// Created by Matt Blair on 7/30/16.
//
#pragma once

#include "gl/GL.hpp"
#include "gl/Pixmap.hpp"
#include <cstdint>

namespace stock {

class RenderState;

class Texture {

public:

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
    MinFilter minFilter = MinFilter::LINEAR;
    MagFilter magFilter = MagFilter::LINEAR;
    Wrap wrapS = Wrap::CLAMP_TO_EDGE;
    Wrap wrapT = Wrap::CLAMP_TO_EDGE;
    bool generateMipmaps = false;
  };

  // Create an empty Texture with default options.
  Texture();

  // Create an empty Texture with custom options.
  Texture(Options options);

  // Create a Texture from image data with custom options.
  Texture(Pixmap pixmap, Options options);

  // Destroy the Texture and free the pixel data.
  ~Texture();

  // Get the image dimensions of the Texture.
  uint32_t width() const;
  uint32_t height() const;

  // Get the OpenGL handle of the Texture.
  GLuint glHandle() const;

  // Initialize OpenGL resources for the Texture, if needed.
  void prepare(RenderState& rs, GLuint unit);

  // Bind the Texture to the specified texture unit.
  void bind(RenderState& rs, GLuint unit);

  // Delete any OpenGL resources for the Texture.
  void dispose(RenderState& rs);

protected:

  // Source pixel data
  Pixmap m_pixmap;

  // Rendering options
  Options m_options;

  // OpenGL texture identifiers
  GLuint m_glHandle = 0;
  GLenum m_target = GL_TEXTURE_2D;
};

} // namespace stock
