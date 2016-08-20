//
// Created by Matt Blair on 8/11/16.
//
#pragma once

#include "gl/Texture.hpp"

namespace stock {

class RenderState;

class Framebuffer {

public:
  using PixelFormat = Texture::PixelFormat;

  struct Options {
    PixelFormat format = PixelFormat::RGBA;
    bool hasDepth = false;
    bool hasStencil = false;
  };

  Framebuffer(uint32_t width, uint32_t height, Options options);

  ~Framebuffer();

  void prepare(RenderState& rs);

  void bind(RenderState& rs);

  void unbind(RenderState& rs);

  void dispose(RenderState& rs);

  Texture& colorTexture();

private:
  static Texture::Options getColorBufferOptions(Options options);

  Texture m_colorTexture;
  GLuint m_framebufferHandle = 0;
  GLuint m_depthbufferHandle = 0;
  GLuint m_stencilbufferHandle = 0;
  uint32_t m_width = 0;
  uint32_t m_height = 0;
  Options m_options;
};

} // namespace stock
