//
// Created by Matt Blair on 4/2/16.
//
#include "gl/ShaderProgram.hpp"
#include "gl/Error.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <algorithm>

namespace stock {

ShaderProgram::ShaderProgram() {}

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

void ShaderProgram::setSourceStrings(const std::string& _fragSrc, const std::string& _vertSrc) {
  m_fragmentShaderSource = std::string(_fragSrc);
  m_vertexShaderSource = std::string(_vertSrc);
  m_needsBuild = true;
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
  assert(location < RenderState::MAX_ATTRIBUTES);
  if (location >= 0) {
    m_attributes[location] = name;
  }
  return location;
}

GLint ShaderProgram::getUniformLocation(const UniformLocation& _uniform) {

  if (m_generation == _uniform.generation) {
    return _uniform.location;
  }

  _uniform.generation = m_generation;
  _uniform.location = glGetUniformLocation(m_glProgram, _uniform.name.c_str());
  CHECK_GL();

  return _uniform.location;
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

  if (m_invalidShaderSource) {
    return false;
  }

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

GLuint ShaderProgram::makeLinkedShaderProgram(GLuint fragShader, GLuint vertShader) {

  GLuint program = glCreateProgram();
  CHECK_GL();
  CHECK_GL(glAttachShader(program, fragShader));
  CHECK_GL(glAttachShader(program, vertShader));
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
    m_invalidShaderSource = true;
    return 0;
  }

  return program;
}

GLuint ShaderProgram::makeCompiledShader(const std::string& _src, GLenum _type) {

  GLuint shader = glCreateShader(_type);
  CHECK_GL();
  const GLchar* source = _src.c_str();
  CHECK_GL(glShaderSource(shader, 1, &source, NULL));
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
    m_invalidShaderSource = true;
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

void ShaderProgram::setUniformi(RenderState& rs, const UniformLocation& _loc, int _value) {
  use(rs);
  GLint location = getUniformLocation(_loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(_value)) {
    CHECK_GL(glUniform1i(location, _value));
  }
}

void ShaderProgram::setUniformi(RenderState& rs, const UniformLocation& _loc, int _value0, int _value1) {
  use(rs);
  GLint location = getUniformLocation(_loc);
  auto& uniform = getCachedUniform(location);
  int values[] = {_value0, _value1};
  if (location >= 0 && uniform.update(values, 2)) {
    CHECK_GL(glUniform2i(location, _value0, _value1));
  }
}

void ShaderProgram::setUniformi(RenderState& rs, const UniformLocation& _loc, int _value0, int _value1, int _value2) {
  use(rs);
  GLint location = getUniformLocation(_loc);
  auto& uniform = getCachedUniform(location);
  int values[] = {_value0, _value1, _value2};
  if (location >= 0 && uniform.update(values, 3)) {
    CHECK_GL(glUniform3i(location, _value0, _value1, _value2));
  }
}

void ShaderProgram::setUniformi(RenderState& rs, const UniformLocation& _loc, int _value0, int _value1, int _value2,
                                int _value3) {
  use(rs);
  GLint location = getUniformLocation(_loc);
  auto& uniform = getCachedUniform(location);
  int values[] = {_value0, _value1, _value2, _value3};
  if (location >= 0 && uniform.update(values, 4)) {
    CHECK_GL(glUniform4i(location, _value0, _value1, _value2, _value3));
  }
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& _loc, float _value) {
  use(rs);
  GLint location = getUniformLocation(_loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(_value)) {
    CHECK_GL(glUniform1f(location, _value));
  }
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& _loc, float _value0, float _value1) {
  setUniformf(rs, _loc, glm::vec2(_value0, _value1));
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& _loc, float _value0, float _value1,
                                float _value2) {
  setUniformf(rs, _loc, glm::vec3(_value0, _value1, _value2));
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& _loc, float _value0, float _value1,
                                float _value2, float _value3) {
  setUniformf(rs, _loc, glm::vec4(_value0, _value1, _value2, _value3));
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& _loc, const glm::vec2& _value) {
  use(rs);
  GLint location = getUniformLocation(_loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(_value), 2)) {
    CHECK_GL(glUniform2f(location, _value.x, _value.y));
  }
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& _loc, const glm::vec3& _value) {
  use(rs);
  GLint location = getUniformLocation(_loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(_value), 3)) {
    CHECK_GL(glUniform3f(location, _value.x, _value.y, _value.z));
  }
}

void ShaderProgram::setUniformf(RenderState& rs, const UniformLocation& _loc, const glm::vec4& _value) {
  use(rs);
  GLint location = getUniformLocation(_loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(_value), 4)) {
    CHECK_GL(glUniform4f(location, _value.x, _value.y, _value.z, _value.w));
  }
}

void ShaderProgram::setUniformMatrix2f(RenderState& rs, const UniformLocation& _loc, const glm::mat2& _value,
                                       bool _transpose) {
  use(rs);
  GLint location = getUniformLocation(_loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(_value), 4)) {
    CHECK_GL(glUniformMatrix2fv(location, 1, _transpose, glm::value_ptr(_value)));
  }
}

void ShaderProgram::setUniformMatrix3f(RenderState& rs, const UniformLocation& _loc, const glm::mat3& _value,
                                       bool _transpose) {
  use(rs);
  GLint location = getUniformLocation(_loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(_value), 9)) {
    CHECK_GL(glUniformMatrix3fv(location, 1, _transpose, glm::value_ptr(_value)));
  }
}

void ShaderProgram::setUniformMatrix4f(RenderState& rs, const UniformLocation& _loc, const glm::mat4& _value,
                                       bool _transpose) {
  use(rs);
  GLint location = getUniformLocation(_loc);
  auto& uniform = getCachedUniform(location);
  if (location >= 0 && uniform.update(glm::value_ptr(_value), 16)) {
    CHECK_GL(glUniformMatrix4fv(location, 1, _transpose, glm::value_ptr(_value)));
  }
}

} // namespace stock
