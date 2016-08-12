//
// Created by Matt Blair on 4/2/16.
//
#pragma once

#include "gl/RenderState.hpp"
#include "gl/ShaderUniform.hpp"
#include "glm/glm.hpp"
#include <array>
#include <string>

namespace stock {

class RenderState;

class ShaderProgram {

public:

    ShaderProgram();
    ~ShaderProgram();

    // Set the vertex and fragment shader GLSL source to the given strings.
    void setSourceStrings(const std::string& fragSrc, const std::string& vertSrc);

    // Attempts to compile and link the vertex and fragment shaders; if
    // compiling or linking fails it prints the compiler log, returns false, and
    // keeps the program's previous state; if successful it returns true.
    bool build(RenderState& rs);

    // Disposes the GL resources for this shader.
    void dispose(RenderState& rs);

    GLuint getGlProgram() const { return m_glProgram; };
    GLuint getGlFragmentShader() const { return m_glFragmentShader; };
    GLuint getGlVertexShader() const { return m_glVertexShader; };

    // Fetch the location of a shader attribute and cache the result.
    GLint getAttributeLocation(const std::string& name);

    // Fetch the location of a shader uniform and cache the result.
    GLint getUniformLocation(const UniformLocation& uniformName);

    // Returns true if this object represents a valid OpenGL shader program.
    bool isValid() const { return m_glProgram != 0; };

    // Bind the program in openGL if it is not already bound; If the shader sources
    // have been modified since the last time build() was called, also calls build().
    // Returns true if shader can be used (i.e. is valid).
    bool use(RenderState& rs);

    // Ensure the program is bound and then set the named uniform to the given value(s).
    void setUniformi(RenderState& rs, const UniformLocation& loc, int value);
    void setUniformi(RenderState& rs, const UniformLocation& loc, int value0, int value1);
    void setUniformi(RenderState& rs, const UniformLocation& loc, int value0, int value1, int value2);
    void setUniformi(RenderState& rs, const UniformLocation& loc, int value0, int value1, int value2, int value3);

    void setUniformf(RenderState& rs, const UniformLocation& loc, float value);
    void setUniformf(RenderState& rs, const UniformLocation& loc, float value0, float value1);
    void setUniformf(RenderState& rs, const UniformLocation& loc, float value0, float value1, float value2);
    void setUniformf(RenderState& rs, const UniformLocation& loc, float value0, float value1, float value2, float value3);

    void setUniformf(RenderState& rs, const UniformLocation& loc, const glm::vec2& value);
    void setUniformf(RenderState& rs, const UniformLocation& loc, const glm::vec3& value);
    void setUniformf(RenderState& rs, const UniformLocation& loc, const glm::vec4& value);

    // Ensures the program is bound and then sets the named uniform to the values
    // beginning at the pointer _value; 4 values are used for a 2x2 matrix, 9 values for a 3x3, etc.
    void setUniformMatrix2f(RenderState& rs, const UniformLocation& loc, const glm::mat2& value, bool transpose = false);
    void setUniformMatrix3f(RenderState& rs, const UniformLocation& loc, const glm::mat3& value, bool transpose = false);
    void setUniformMatrix4f(RenderState& rs, const UniformLocation& loc, const glm::mat4& value, bool transpose = false);

private:

    std::array<std::string, RenderState::MAX_ATTRIBUTES> m_attributes;

    std::string m_fragmentShaderSource;
    std::string m_vertexShaderSource;

    int32_t m_generation = -1;
    GLuint m_glProgram = 0;
    GLuint m_glFragmentShader = 0;
    GLuint m_glVertexShader = 0;

    bool m_needsBuild = true;
    bool m_invalidShaderSource = false;

    void checkValidity(RenderState& renderState);
    GLuint makeLinkedShaderProgram(GLuint fragShader, GLuint vertShader);
    GLuint makeCompiledShader(const std::string& src, GLenum type);

};

} // namespace stock

