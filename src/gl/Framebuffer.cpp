//
// Created by Matt Blair on 8/11/16.
//
#include "gl/Framebuffer.hpp"
#include "gl/Error.hpp"
#include "io/Log.hpp"

namespace stock {

Framebuffer::Framebuffer(uint32_t width, uint32_t height, Options options)
    : m_colorTexture(Pixmap(width, height, nullptr, options.format), Texture::Options()),
      m_width(width), m_height(height), m_options(options) {}

Framebuffer::~Framebuffer() {}

void Framebuffer::prepare(RenderState& rs) {

  m_colorTexture.prepare(rs);

  // Generate framebuffer handle.
  CHECK_GL(glGenFramebuffers(1, &m_framebufferHandle));

  if (m_options.hasDepth) {
    // Generate depth buffer handle.
    CHECK_GL(glGenRenderbuffers(1, &m_depthbufferHandle));
  }

  if (m_options.hasStencil) {
    // Generate stencil buffer handle.
    CHECK_GL(glGenRenderbuffers(1, &m_stencilbufferHandle));
  }

  // Bind color texture.
  m_colorTexture.bind(rs);

  if (m_options.hasDepth) {
    // Create depth buffer storage.
    CHECK_GL(glBindRenderbuffer(GL_RENDERBUFFER, m_depthbufferHandle));
    CHECK_GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_width, m_height));
  }

  if (m_options.hasStencil) {
    // Create stencil buffer storage.
    CHECK_GL(glBindRenderbuffer(GL_RENDERBUFFER, m_stencilbufferHandle));
    CHECK_GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, m_width, m_height));
  }

  // Bind the framebuffer.
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle));

  // Attach the color buffer.
  CHECK_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture.glHandle(), 0));

  if (m_options.hasDepth) {
    // Attach the depth buffer.
    CHECK_GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbufferHandle));
  }

  if (m_options.hasStencil) {
    // Attach the stencil buffer.
    CHECK_GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_stencilbufferHandle));
  }

  // Unbind renderbuffer object.
  CHECK_GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

  // Check framebuffer status.
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  CHECK_GL();

  // Bind the default framebuffer object.
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

  // Check status and dispose on failure.
  if (status != GL_FRAMEBUFFER_COMPLETE) {

    dispose(rs);

    switch (status) {
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      Log::e("Error creating framebuffer: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      Log::e("Error creating framebuffer: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
      Log::e("Error creating framebuffer: GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      Log::e("Error creating framebuffer: GL_FRAMEBUFFER_UNSUPPORTED");
      break;
    default:
      Log::e("Error creating framebuffer: Unrecognized error.");
      break;
    }
  }
}

void Framebuffer::bind(RenderState& rs) {

  if (m_framebufferHandle == 0) {
    prepare(rs);
  }

  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferHandle));
}

void Framebuffer::unbind(RenderState& rs) {
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Framebuffer::dispose(RenderState& rs) {

  m_colorTexture.dispose(rs);

  if (m_options.hasDepth && m_depthbufferHandle != 0) {
    CHECK_GL(glDeleteRenderbuffers(1, &m_depthbufferHandle));
    m_depthbufferHandle = 0;
  }

  if (m_options.hasStencil && m_stencilbufferHandle != 0) {
    CHECK_GL(glDeleteRenderbuffers(1, &m_stencilbufferHandle));
    m_stencilbufferHandle = 0;
  }

  if (m_framebufferHandle != 0) {
    CHECK_GL(glDeleteFramebuffers(1, &m_framebufferHandle));
    m_framebufferHandle = 0;
  }
}

Texture& Framebuffer::colorTexture() {
  return m_colorTexture;
}

} // namespace stock
