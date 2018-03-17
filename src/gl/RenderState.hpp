//
// Created by Matt Blair on 4/2/16.
//
#pragma once

#include "gl/GL.hpp"
#include <array>

namespace stock {

class RenderState {

public:
  static constexpr size_t MAX_ATTRIBUTES = 16;

  // TODO: read max texture units from hardware
  static constexpr size_t MAX_COMBINED_TEXTURE_UNITS = 16;

  RenderState() = default;

  // Disallow copying by construction and assignment.
  RenderState(const RenderState&) = delete;
  RenderState& operator=(const RenderState&) = delete;

  // Reset the render states.
  void reset();

  bool blending(GLboolean enable);

  bool blendingFunc(GLenum sfactor, GLenum dfactor);

  bool clearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);

  bool colorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a);

  bool cullFace(GLenum face);

  bool culling(GLboolean enable);

  bool depthTest(GLboolean enable);

  bool depthMask(GLboolean enable);

  bool frontFace(GLenum face);

  bool stencilMask(GLuint mask);

  bool stencilFunc(GLenum func, GLint ref, GLuint mask);

  bool stencilOp(GLenum sfail, GLenum spassdfail, GLenum spassdpass);

  bool stencilTest(GLboolean enable);

  bool shaderProgram(GLuint program);

  bool texture(GLenum target, GLuint unit, GLuint handle);

  bool textureUnit(GLuint unit);

  bool vertexBuffer(GLuint handle);

  bool indexBuffer(GLuint handle);

  void vertexBufferUnset(GLuint handle);

  void indexBufferUnset(GLuint handle);

  void shaderProgramUnset(GLuint program);

  void textureUnset(GLenum target, GLuint handle);

  // For each vertex attribute location, contains the GL program last used to bind the attribute or zero if unbound.
  std::array<GLuint, MAX_ATTRIBUTES> attributeBindings = {{0}};

private:

  std::array<GLuint, MAX_COMBINED_TEXTURE_UNITS> textureBindings = {{0}};

  struct {
    GLboolean enabled;
    bool set;
  } m_blending, m_culling, m_depthMask, m_depthTest, m_stencilTest;

  struct {
    GLenum sfactor, dfactor;
    bool set;
  } m_blendingFunc;

  struct {
    GLuint mask;
    bool set;
  } m_stencilMask;

  struct {
    GLenum func;
    GLint ref;
    GLuint mask;
    bool set;
  } m_stencilFunc;

  struct {
    GLenum sfail, spassdfail, spassdpass;
    bool set;
  } m_stencilOp;

  struct {
    GLboolean r, g, b, a;
    bool set;
  } m_colorMask;

  struct {
    GLenum face;
    bool set;
  } m_frontFace, m_cullFace;

  struct {
    GLuint handle;
    bool set;
  } m_vertexBuffer, m_indexBuffer;

  struct {
    GLuint program;
    bool set;
  } m_program;

  struct {
    GLclampf r, g, b, a;
    bool set;
  } m_clearColor;

  struct {
    GLuint unit;
    bool set;
  } m_textureUnit;
};

} // namespace stock
