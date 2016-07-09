//
// Created by Matt Blair on 4/2/16.
//
#include "gl/Error.hpp"
#include "gl/RenderState.hpp"
#include "gl/ShaderProgram.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace stock {

ShaderProgram::ShaderProgram() {

    m_glProgram = 0;
    m_glFragmentShader = 0;
    m_glVertexShader = 0;
    m_needsBuild = true;
    m_generation = -1;
    m_invalidShaderSource = false;
}

ShaderProgram::~ShaderProgram() {

    if (m_glProgram != 0) {
        CHECK_GL(glDeleteProgram(m_glProgram));
    }

    if (m_glFragmentShader != 0) {
        CHECK_GL(glDeleteShader(m_glFragmentShader));
    }

    if (m_glVertexShader != 0) {
        CHECK_GL(glDeleteShader(m_glVertexShader));
    }

    // Deleting a shader program being used ends up setting up the current shader program to 0
    // after the driver finishes using it, force this setup by setting the current program
    if (RenderState::shaderProgram.compare(m_glProgram)) {
        RenderState::shaderProgram.init(0, false);
    }

}

void ShaderProgram::setSourceStrings(const std::string& _fragSrc, const std::string& _vertSrc){
    m_fragmentShaderSource = std::string(_fragSrc);
    m_vertexShaderSource = std::string(_vertSrc);
    m_needsBuild = true;
}

GLint ShaderProgram::getAttribLocation(const std::string& _attribName) {

    auto it = m_attribMap.find(_attribName);

    if (it == m_attribMap.end()) {
        // If this is a new entry, get the actual location from OpenGL.
        GLint location = glGetAttribLocation(m_glProgram, _attribName.c_str());
        CHECK_GL();
        m_attribMap[_attribName] = location;
        return location;
    } else {
        return it->second;
    }
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

bool ShaderProgram::use() {

    checkValidity();

    if (m_needsBuild) {
        build();
    }

    if (m_glProgram != 0) {
        RenderState::shaderProgram(m_glProgram);
        return true;
    }
    return false;
}

bool ShaderProgram::build() {

    m_needsBuild = false;
    m_generation = RenderState::generation();

    if (m_invalidShaderSource) { return false; }

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

    m_attribMap.clear();

    return true;
}

GLuint ShaderProgram::makeLinkedShaderProgram(GLuint fragShader, GLuint vertShader) {

    GLuint program = glCreateProgram();
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
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

    if (isCompiled == GL_FALSE) {
        GLint infoLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);
        if (infoLength > 1) {
            std::vector<GLchar> infoLog(infoLength);
            glGetShaderInfoLog(shader, infoLength, NULL, &infoLog[0]);
            Log::ef("Error compiling shader:\n%s", &infoLog[0]);
        }
        glDeleteShader(shader);
        m_invalidShaderSource = true;
        return 0;
    }

    return shader;

}

void ShaderProgram::checkValidity() {

    if (!RenderState::isValidGeneration(m_generation)) {
        m_glFragmentShader = 0;
        m_glVertexShader = 0;
        m_glProgram = 0;
        m_needsBuild = true;
    }
}

void ShaderProgram::setUniformi(const UniformLocation& _loc, int _value) {
    use();
    GLint location = getUniformLocation(_loc);
    if (location >= 0) {
        glUniform1i(location, _value);
    }
}

void ShaderProgram::setUniformi(const UniformLocation& _loc, int _value0, int _value1) {
    use();
    GLint location = getUniformLocation(_loc);
    if (location >= 0) {
        glUniform2i(location, _value0, _value1);
    }
}

void ShaderProgram::setUniformi(const UniformLocation& _loc, int _value0, int _value1, int _value2) {
    use();
    GLint location = getUniformLocation(_loc);
    if (location >= 0) {
        glUniform3i(location, _value0, _value1, _value2);
    }
}

void ShaderProgram::setUniformi(const UniformLocation& _loc, int _value0, int _value1, int _value2, int _value3) {
    use();
    GLint location = getUniformLocation(_loc);
    if (location >= 0) {
        glUniform4i(location, _value0, _value1, _value2, _value3);
    }
}

void ShaderProgram::setUniformf(const UniformLocation& _loc, float _value) {
    use();
    GLint location = getUniformLocation(_loc);
    if (location >= 0) {
        glUniform1f(location, _value);
    }
}

void ShaderProgram::setUniformf(const UniformLocation& _loc, float _value0, float _value1) {
    setUniformf(_loc, glm::vec2(_value0, _value1));
}

void ShaderProgram::setUniformf(const UniformLocation& _loc, float _value0, float _value1, float _value2) {
    setUniformf(_loc, glm::vec3(_value0, _value1, _value2));
}

void ShaderProgram::setUniformf(const UniformLocation& _loc, float _value0, float _value1, float _value2, float _value3) {
    setUniformf(_loc, glm::vec4(_value0, _value1, _value2, _value3));
}

void ShaderProgram::setUniformf(const UniformLocation& _loc, const glm::vec2& _value) {
    use();
    GLint location = getUniformLocation(_loc);
    if (location >= 0) {
        glUniform2f(location, _value.x, _value.y);
    }
}

void ShaderProgram::setUniformf(const UniformLocation& _loc, const glm::vec3& _value) {
    use();
    GLint location = getUniformLocation(_loc);
    if (location >= 0) {
        glUniform3f(location, _value.x, _value.y, _value.z);
    }
}

void ShaderProgram::setUniformf(const UniformLocation& _loc, const glm::vec4& _value) {
    use();
    GLint location = getUniformLocation(_loc);
    if (location >= 0) {
        glUniform4f(location, _value.x, _value.y, _value.z, _value.w);
    }
}

void ShaderProgram::setUniformMatrix2f(const UniformLocation& _loc, const glm::mat2& _value, bool _transpose) {
    use();
    GLint location = getUniformLocation(_loc);
    if (location >= 0) {
        glUniformMatrix2fv(location, 1, _transpose, glm::value_ptr(_value));
    }
}

void ShaderProgram::setUniformMatrix3f(const UniformLocation& _loc, const glm::mat3& _value, bool _transpose) {
    use();
    GLint location = getUniformLocation(_loc);
    if (location >= 0) {
        glUniformMatrix3fv(location, 1, _transpose, glm::value_ptr(_value));
    }
}

void ShaderProgram::setUniformMatrix4f(const UniformLocation& _loc, const glm::mat4& _value, bool _transpose) {
    use();
    GLint location = getUniformLocation(_loc);
    if (location >= 0) {
        glUniformMatrix4fv(location, 1, _transpose, glm::value_ptr(_value));
    }
}

//void ShaderProgram::setUniformf(const UniformLocation& _loc, const UniformArray& _value) {
//    use();
//    GLint location = getUniformLocation(_loc);
//    if (location >= 0) {
//        bool cached = getFromCache(location, _value);
//        if (!cached) { glUniform1fv(location, _value.size(), _value.data()); }
//    }
//}
//
//void ShaderProgram::setUniformi(const UniformLocation& _loc, const UniformTextureArray& _value) {
//    use();
//    GLint location = getUniformLocation(_loc);
//    if (location >= 0) {
//        bool cached = getFromCache(location, _value);
//        if (!cached) { glUniform1iv(location, _value.slots.size(), _value.slots.data()); }
//    }
//}

} // namespace stock
