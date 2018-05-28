//
// Created by Matt Blair on 4/2/16.
//
#include "gl/Error.hpp"
#include "gl/RenderState.hpp"

namespace stock {

void RenderState::reset() {

  m_blending.set = false;
  m_blendEquation.set = false;
  m_blendFunc.set = false;
  m_clearColor.set = false;
  m_colorMask.set = false;
  m_cullFace.set = false;
  m_culling.set = false;
  m_depthTest.set = false;
  m_depthMask.set = false;
  m_frontFace.set = false;
  m_stencilTest.set = false;
  m_stencilMask.set = false;
  m_scissorTest.set = false;
  m_scissor.set = false;
  m_program.set = false;
  m_indexBuffer.set = false;
  m_vertexBuffer.set = false;
  m_textureUnit.set = false;

  attributeBindings.fill(0);
  textureBindings.fill(0);

  CHECK_GL(glDepthFunc(GL_LESS));
}

inline void setGlFlag(GLenum flag, GLboolean enable) {
  if (enable) {
    CHECK_GL(glEnable(flag));
  } else {
    CHECK_GL(glDisable(flag));
  }
}

bool RenderState::blending(GLboolean enable) {
  if (!m_blending.set || m_blending.enabled != enable) {
    m_blending = {enable, true};
    setGlFlag(GL_BLEND, enable);
    return false;
  }
  return true;
}

bool RenderState::blendEquation(GLenum mode) {
  if (!m_blendEquation.set || m_blendEquation.mode != mode) {
    m_blendEquation = {mode, true};
    CHECK_GL(glBlendEquation(mode));
    return false;
  }
  return true;
}

bool RenderState::blendFunc(GLenum sfactor, GLenum dfactor) {
  if (!m_blendFunc.set || m_blendFunc.sfactor != sfactor || m_blendFunc.dfactor != dfactor) {
    m_blendFunc = {sfactor, dfactor, true};
    CHECK_GL(glBlendFunc(sfactor, dfactor));
    return false;
  }
  return true;
}

bool RenderState::clearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a) {
  if (!m_clearColor.set || m_clearColor.r != r || m_clearColor.g != g || m_clearColor.b != b || m_clearColor.a != a) {
    m_clearColor = {r, g, b, a, true};
    CHECK_GL(glClearColor(r, g, b, a));
    return false;
  }
  return true;
}

bool RenderState::colorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a) {
  if (!m_colorMask.set || m_colorMask.r != r || m_colorMask.g != g || m_colorMask.b != b || m_colorMask.a != a) {
    m_colorMask = {r, g, b, a, true};
    CHECK_GL(glColorMask(r, g, b, a));
    return false;
  }
  return true;
}

bool RenderState::cullFace(GLenum face) {
  if (!m_cullFace.set || m_cullFace.face != face) {
    m_cullFace = {face, true};
    CHECK_GL(glCullFace(face));
    return false;
  }
  return true;
}

bool RenderState::culling(GLboolean enable) {
  if (!m_culling.set || m_culling.enabled != enable) {
    m_culling = {enable, true};
    setGlFlag(GL_CULL_FACE, enable);
    return false;
  }
  return true;
}

bool RenderState::depthTest(GLboolean enable) {
  if (!m_depthTest.set || m_depthTest.enabled != enable) {
    m_depthTest = {enable, true};
    setGlFlag(GL_DEPTH_TEST, enable);
    return false;
  }
  return true;
}

bool RenderState::depthMask(GLboolean enable) {
  if (!m_depthMask.set || m_depthMask.enabled != enable) {
    m_depthMask = {enable, true};
    CHECK_GL(glDepthMask(enable));
    return false;
  }
  return true;
}

bool RenderState::frontFace(GLenum face) {
  if (!m_frontFace.set || m_frontFace.face != face) {
    m_frontFace = {face, true};
    CHECK_GL(glFrontFace(face));
    return false;
  }
  return true;
}

bool RenderState::stencilMask(GLuint mask) {
  if (!m_stencilMask.set || m_stencilMask.mask != mask) {
    m_stencilMask = {mask, true};
    CHECK_GL(glStencilMask(mask));
    return false;
  }
  return true;
}

bool RenderState::stencilFunc(GLenum func, GLint ref, GLuint mask) {
  if (!m_stencilFunc.set || m_stencilFunc.func != func || m_stencilFunc.ref != ref || m_stencilFunc.mask != mask) {
    m_stencilFunc = {func, ref, mask, true};
    CHECK_GL(glStencilFunc(func, ref, mask));
    return false;
  }
  return true;
}

bool RenderState::stencilOp(GLenum sfail, GLenum spassdfail, GLenum spassdpass) {
  if (!m_stencilOp.set || m_stencilOp.sfail != sfail || m_stencilOp.spassdfail != spassdfail ||
      m_stencilOp.spassdpass != spassdpass) {
    m_stencilOp = {sfail, spassdfail, spassdpass, true};
    CHECK_GL(glStencilOp(sfail, spassdfail, spassdpass));
    return false;
  }
  return true;
}

bool RenderState::stencilTest(GLboolean enable) {
  if (!m_stencilTest.set || m_stencilTest.enabled != enable) {
    m_stencilTest = {enable, true};
    setGlFlag(GL_STENCIL_TEST, enable);
    return false;
  }
  return true;
}

bool RenderState::scissor(GLint x, GLint y, GLsizei width, GLsizei height) {
  if (!m_scissor.set || x != m_scissor.x || y != m_scissor.y || width != m_scissor.width ||
      height != m_scissor.height) {
    m_scissor = {x, y, width, height, true};
    CHECK_GL(glScissor(x, y, width, height));
    return false;
  }
  return true;
}

bool RenderState::scissorTest(GLboolean enable) {
  if (!m_scissorTest.set || m_scissorTest.enabled != enable) {
    m_scissorTest = {enable, true};
    setGlFlag(GL_SCISSOR_TEST, enable);
    return false;
  }
  return true;
}

bool RenderState::shaderProgram(GLuint program) {
  if (!m_program.set || m_program.program != program) {
    m_program = {program, true};
    CHECK_GL(glUseProgram(program));
    return false;
  }
  return true;
}

bool RenderState::texture(GLenum target, GLuint unit, GLuint handle) {
  textureUnit(unit);
  if (textureBindings[unit] != handle) {
    textureBindings[unit] = handle;
    CHECK_GL(glBindTexture(target, handle));
    return false;
  }
  return true;
}

bool RenderState::textureUnit(GLuint unit) {
  if (!m_textureUnit.set || m_textureUnit.unit != unit) {
    m_textureUnit = {unit, true};
    CHECK_GL(glActiveTexture(GL_TEXTURE0 + unit));
    return false;
  }
  return true;
}

bool RenderState::vertexBuffer(GLuint handle) {
  if (!m_vertexBuffer.set || m_vertexBuffer.handle != handle) {
    m_vertexBuffer = {handle, true};
    CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, handle));
    return false;
  }
  return true;
}

bool RenderState::indexBuffer(GLuint handle) {
  if (!m_indexBuffer.set || m_indexBuffer.handle != handle) {
    m_indexBuffer = {handle, true};
    CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle));
    return false;
  }
  return true;
}

void RenderState::vertexBufferUnset(GLuint handle) {
  if (m_vertexBuffer.handle == handle) {
    m_vertexBuffer.set = false;
  }
}

void RenderState::indexBufferUnset(GLuint handle) {
  if (m_indexBuffer.handle == handle) {
    m_indexBuffer.set = false;
  }
}

void RenderState::shaderProgramUnset(GLuint program) {
  if (m_program.program == program) {
    m_program.set = false;
  }
}

void RenderState::textureUnset(GLenum target, GLuint handle) {
  for (auto& binding : textureBindings) {
    if (binding == handle) {
      binding = 0;
    }
  }
}

} // namespace stock
