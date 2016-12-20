//
// Created by Matt Blair on 4/2/16.
//
#pragma once

#include "gl/RenderState.hpp"
#include "gl/ShaderUniform.hpp"
#include "glm/glm.hpp"
#include <array>
#include <string>
#include <vector>

namespace stock {

class RenderState;

class ShaderProgram {

public:
  // Create a shader program the given vertex and fragment shader GLSL sources.
  ShaderProgram(const std::string& fragmentShaderSource, const std::string& vertexShaderSource);
  ~ShaderProgram();

  // Attempts to compile and link the vertex and fragment shaders; if
  // compiling or linking fails it prints the compiler log, returns false, and
  // keeps the program's previous state; if successful it returns true.
  bool build(RenderState& rs);

  // Disposes the GL resources for this shader.
  void dispose(RenderState& rs);

  GLuint getGlProgram() const;
  GLuint getGlFragmentShader() const;
  GLuint getGlVertexShader() const;

  // Fetch the location of a shader attribute and cache the result.
  GLint getAttributeLocation(const std::string& name);

  // Fetch the location of a shader uniform and cache the result.
  GLint getUniformLocation(const UniformLocation& uniform);

  // Returns true if this object represents a valid OpenGL shader program.
  bool isValid() const;

  // Bind the program in openGL if it is not already bound; If the shader sources
  // have been modified since the last time build() was called, also calls build().
  // Returns true if shader can be used (i.e. is valid).
  bool use(RenderState& rs);

  // Ensure the program is bound and then set the named uniform to the given value(s).
  void setUniformi(RenderState& rs, const UniformLocation& loc, int v0);
  void setUniformi(RenderState& rs, const UniformLocation& loc, int v0, int v1);
  void setUniformi(RenderState& rs, const UniformLocation& loc, int v0, int v1, int v2);
  void setUniformi(RenderState& rs, const UniformLocation& loc, int v0, int v1, int v2, int v3);

  void setUniformf(RenderState& rs, const UniformLocation& loc, float v0);
  void setUniformf(RenderState& rs, const UniformLocation& loc, float v0, float v1);
  void setUniformf(RenderState& rs, const UniformLocation& loc, float v0, float v1, float v2);
  void setUniformf(RenderState& rs, const UniformLocation& loc, float v0, float v1, float v2, float v3);

  void setUniformf(RenderState& rs, const UniformLocation& loc, const glm::vec2& v0);
  void setUniformf(RenderState& rs, const UniformLocation& loc, const glm::vec3& v0);
  void setUniformf(RenderState& rs, const UniformLocation& loc, const glm::vec4& v0);

  // Ensures the program is bound and then sets the named uniform to the values
  // beginning at the pointer _value; 4 values are used for a 2x2 matrix, 9 values for a 3x3, etc.
  void setUniformMatrix2f(RenderState& rs, const UniformLocation& loc, const glm::mat2& v0);
  void setUniformMatrix3f(RenderState& rs, const UniformLocation& loc, const glm::mat3& v0);
  void setUniformMatrix4f(RenderState& rs, const UniformLocation& loc, const glm::mat4& v0);

private:
  std::vector<ShaderUniform> m_uniformCache;
  std::array<std::string, RenderState::MAX_ATTRIBUTES> m_attributes;

  std::string m_fragmentShaderSource;
  std::string m_vertexShaderSource;

  int32_t m_generation = -1;
  GLuint m_glProgram = 0;
  GLuint m_glFragmentShader = 0;
  GLuint m_glVertexShader = 0;

  bool m_needsBuild = true;

  void checkValidity(RenderState& renderState);
  GLuint makeLinkedShaderProgram(GLuint fragmentShader, GLuint vertexShader);
  GLuint makeCompiledShader(const std::string& source, GLenum type);
  ShaderUniform& getCachedUniform(GLint location);
};

} // namespace stock
