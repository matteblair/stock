//
// Created by Matt Blair on 4/2/16.
//
#include "gl/ShaderProgram.hpp"
#include "gl/VertexLayout.hpp"
#include "gl/Error.hpp"

namespace stock {

std::map<GLuint, GLuint> VertexLayout::s_enabledAttributes;

VertexLayout::VertexLayout(std::vector<VertexAttribute> attributes) : m_attributes(attributes) {

    for (auto& a : m_attributes) {
        a.offset = m_stride;
        GLint bytes = a.size;
        switch (a.type) {
            case GL_FLOAT:
            case GL_INT:
            case GL_UNSIGNED_INT:
                bytes *= 4; // 4 bytes for floats, ints, and uints
                break;
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
                bytes *= 2; // 2 bytes for shorts and ushorts
                break;
            default:
                break;
        }
        m_stride += bytes;
    }

}

size_t VertexLayout::getOffset(const std::string& attributeName) const {

    for (const auto& a : m_attributes) {
        if (a.name == attributeName) {
            return a.offset;
        }
    }

    return 0;
}

void VertexLayout::enable(ShaderProgram& program, size_t offset) {

    GLuint glProgram = program.getGlProgram();

    // Enable all attributes for this layout.
    for (auto& a : m_attributes) {

        GLint location = program.getAttribLocation(a.name);

        if (location >= 0) {
            GLuint& boundProgram = s_enabledAttributes[location];
            // Track currently enabled attributes by the program to which they are bound.
            if (boundProgram != glProgram) {
                CHECK_GL(glEnableVertexAttribArray(location));
                boundProgram = glProgram;
            }

            void* data = reinterpret_cast<void*>(a.offset + offset);
            CHECK_GL(glVertexAttribPointer(location, a.size, a.type, a.normalized, m_stride, data));
        }
    }

    // Disable previously bound and now-unneeded attributes
    for (auto& locationProgramPair : s_enabledAttributes) {

        const GLuint& location = locationProgramPair.first;
        GLuint& boundProgram = locationProgramPair.second;

        if (boundProgram != glProgram && boundProgram != 0) {
            CHECK_GL(glDisableVertexAttribArray(location));
            boundProgram = 0;
        }
    }
}

void VertexLayout::clearCache() {
    s_enabledAttributes.clear();
}

} // namespace stock
