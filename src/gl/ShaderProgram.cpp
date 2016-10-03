//
// Created by Matt Blair on 4/2/16.
//
#include "gl/ShaderProgram.hpp"
#include "gl/Error.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <algorithm>

namespace stock {

ShaderProgram::ShaderProgram(const std::string& fragmentShaderSource, const std::string& vertexShaderSource)
    : m_fragmentShaderSource(fragmentShaderSource), m_vertexShaderSource(vertexShaderSource) {}

ShaderProgram::~ShaderProgram() {}

void ShaderProgram::dispose(RenderState& rs) {
  if (m_glProgram != 0) {
    CHECK_GL(glDeleteProgram(m_glProgram));
  }

  if (m_glFragmentShader != 0) {
    CHECK_GL(glDeleteShader(m_glFragmentShader));
  }

  if (m_glVertexShader != 0) {
    CHECK_GL(glDeleteShader(m_glVertexShader));
  }

  rs.shaderProgramUnset(m_glProgram);
}

GLint ShaderProgram::getAttributeLocation(const std::string& name) {

  auto maxAttributes = RenderState::MAX_ATTRIBUTES;
  for (size_t i = 0; i < maxAttributes; ++i) {
    if (m_attributes[i] == name) {
      return static_cast<GLint>(i);
    }
  }

  // If this is a new entry, get the actual location from OpenGL.
  GLint location = glGetAttribLocation(m_glProgram, name.c_str());
  CHECK_GL();
  assert(location < RenderState::MAX_ATTRIBUTES || location < 0);
  if (location >= 0) {
    m_attributes[location] = name;
  }
  return location;
}

GLint ShaderProgram::getUniformLocation(const UniformLocation& uniform) {

  if (m_generation == uniform.generation) {
    return uniform.location;
  }

  uniform.generation = m_generation;
  uniform.location = glGetUniformLocation(m_glProgram, uniform.name.c_str());
  CHECK_GL();

  return uniform.location;
}

GLuint ShaderProgram::getGlProgram() const {
  return m_glProgram;
}

GLuint ShaderProgram::getGlFragmentShader() const {
  return m_glFragmentShader;
}

GLuint ShaderProgram::getGlVertexShader() const {
  return m_glVertexShader;
}

bool ShaderProgram::isValid() const {
  return m_glProgram != 0;
}

bool ShaderProgram::use(RenderState& rs) {

  checkValidity(rs);

  if (m_needsBuild) {
    build(rs);
  }

  if (m_glProgram != 0) {
    rs.shaderProgram(m_glProgram);
    return true;
  }
  return false;
}

bool ShaderProgram::build(RenderState& rs) {

  m_needsBuild = false;
  m_generation = rs.generation();

  // Try to compile vertex and fragment shaders, releasing resources and
  // quiting on failure.

  GLuint vertexShader = makeCompiledShader(m_vertexShaderSource, GL_VERTEX_SHADER);

  if (vertexShader == 0) {
    return false;
  }

  GLuint fragmentShader = makeCompiledShader(m_fragmentShaderSource, GL_FRAGMENT_SHADER);

  if (fragmentShader == 0) {
    CHECK_GL(glDeleteShader(vertexShader));
    return false;
  }

  // Try to link shaders into a program, releasing resources and
  // quiting on failure.

  GLuint program = makeLinkedShaderProgram(fragmentShader, vertexShader);

  if (program == 0) {
    CHECK_GL(glDeleteShader(vertexShader));
    CHECK_GL(glDeleteShader(fragmentShader));
    return false;
  }

  // Delete handles for old shaders and program;
  // values of 0 are silently ignored.

  CHECK_GL(glDeleteShader(m_glFragmentShader));
  CHECK_GL(glDeleteShader(m_glVertexShader));
  CHECK_GL(glDeleteProgram(m_glProgram));

  m_glFragmentShader = fragmentShader;
  m_glVertexShader = vertexShader;
  m_glProgram = program;

  // Clear any cached shader locations.

  m_attributes.fill("");

  return true;
}

GLuint ShaderProgram::makeLinkedShaderProgram(GLuint fragmentShader, GLuint vertexShader) {

  GLuint program = glCreateProgram();
  CHECK_GL();
  CHECK_GL(glAttachShader(program, fragmentShader));
  CHECK_GL(glAttachShader(program, vertexShader));
  CHECK_GL(glLinkProgram(program));

  GLint isLinked;
  CHECK_GL(glGetProgramiv(program, GL_LINK_STATUS, &isLinked));

  if (isLinked == GL_FALSE) {
    GLint infoLength = 0;
    CHECK_GL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLength));
    if (infoLength > 1) {
      std::vector<GLchar> infoLog(infoLength);
      CHECK_GL(glGetProgramInfoLog(program, infoLength, NULL, &infoLog[0]));
      Log::ef("Error linking program:\n%s", &infoLog[0]);
    }
    CHECK_GL(glDeleteProgram(program));
    return 0;
  }

  return program;
}

GLuint ShaderProgram::makeCompiledShader(const std::string& source, GLenum type) {

  GLuint shader = glCreateShader(type);
  CHECK_GL();
  const GLchar* sourcePtr = source.c_str();
  const GLint sourceLength = source.size();
  CHECK_GL(glShaderSource(shader, 1, &sourcePtr, &sourceLength));
  CHECK_GL(glCompileShader(shader));

  GLint isCompiled;
  CHECK_GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled));

  if (isCompiled == GL_FALSE) {
    GLint infoLength = 0;
    CHECK_GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength));
    if (infoLength > 1) {
      std::vector<GLchar> infoLog(infoLength);
      CHECK_GL(glGetShaderInfoLog(shader, infoLength, NULL, &infoLog[0]));
      Log::ef("Error compiling shader:\n%s", &infoLog[0]);
    }
    CHECK_GL(glDeleteShader(shader));
    return 0;
  }

  return shader;
}

void ShaderProgram::checkValidity(RenderState& renderState) {

  if (!renderState.isValidGeneration(m_generation)) {
    m_glFragmentShader = 0;
    m_glVertexShader = 0;
    m_glProgram = 0;
    m_needsBuild = true;
  }
}

bool uniformCacheCompare(const ShaderUniform& u, int location) {
  return u.location() < location;
}

ShaderUniform& ShaderProgram::getCachedUniform(GLint location) {
  auto it = std::lower_bound(m_uniformCache.begin(), m_uniformCache.end(), location, &uniformCacheCompare);
  if (it != m_uniformCache.end() && it->location() == location) {
    return *it;
  }
  return *m_uniformCache.emplace(it, location);
}

void ShaderProgram::setUniformi(RenderState& rs, const UniformLocation& loc, int v0) {
  use(rs);
  GLint location = getUniformLocation(loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(v0)) {
    CHECK_GL(glUniform1i(location, v0));
  }
}

void ShaderProgram::setUniformi(RenderState& rs, const UniformLocation& loc, int v0, int v1) {
  use(rs);
  GLint location = getUniformLocation(loc);
  auto& uniform = getCachedUniform(location);
  int values[] = {v0, v1};
  if (location >= 0 && uniform.update(values, 2)) {
    CHECK_GL(glUniform2i(location, v0, v1));
  }
}

void ShaderProgram::setUniformi(RenderState& rs, const UniformLocation& loc, int v0, int v1, int v2) {
  use(rs);
  GLint location = getUniformLocation(loc);
  auto& uniform = getCachedUniform(location);
  int values[] = {v0, v1, v2};
  if (location >= 0 && uniform.update(values, 3)) {
    CHECK_GL(glUniform3i(location, v0, v1, v2));
  }
}

void ShaderProgram::setUniformi(RenderState& rs, const UniformLocation& loc, int v0, int v1, int v2, int v3) {
  use(rs);
  GLint location = getUniformLocation(loc);
  auto& uniform = getCachedUniform(location);
  int values[] = {v0, v1, v2, v3};
  if (location >= 0 && uniform.update(values, 4)) {
    CHECK_GL(glUniform4i(location, v0, v1, v2, v3));
  }
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& loc, float v0) {
  use(rs);
  GLint location = getUniformLocation(loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(v0)) {
    CHECK_GL(glUniform1f(location, v0));
  }
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& loc, float v0, float v1) {
  setUniformf(rs, loc, glm::vec2(v0, v1));
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& loc, float v0, float v1, float v2) {
  setUniformf(rs, loc, glm::vec3(v0, v1, v2));
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& loc, float v0, float v1, float v2, float v3) {
  setUniformf(rs, loc, glm::vec4(v0, v1, v2, v3));
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& loc, const glm::vec2& value) {
  use(rs);
  GLint location = getUniformLocation(loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(value), 2)) {
    CHECK_GL(glUniform2f(location, value.x, value.y));
  }
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& loc, const glm::vec3& value) {
  use(rs);
  GLint location = getUniformLocation(loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(value), 3)) {
    CHECK_GL(glUniform3f(location, value.x, value.y, value.z));
  }
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& loc, const glm::vec4& value) {
  use(rs);
  GLint location = getUniformLocation(loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(value), 4)) {
    CHECK_GL(glUniform4f(location, value.x, value.y, value.z, value.w));
  }
}

void ShaderProgram::setUniformMatrix2f(RenderState& rs, const UniformLocation& loc, const glm::mat2& value) {
  use(rs);
  GLint location = getUniformLocation(loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(value), 4)) {
    CHECK_GL(glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value)));
  }
}

void ShaderProgram::setUniformMatrix3f(RenderState& rs, const UniformLocation& loc, const glm::mat3& value) {
  use(rs);
  GLint location = getUniformLocation(loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(value), 9)) {
    CHECK_GL(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value)));
  }
}

void ShaderProgram::setUniformMatrix4f(RenderState& rs, const UniformLocation& loc, const glm::mat4& value) {
  use(rs);
  GLint location = getUniformLocation(loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(value), 16)) {
    CHECK_GL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value)));
  }
}

} // namespace stock
