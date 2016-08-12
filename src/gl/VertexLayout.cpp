//
// Created by Matt Blair on 4/2/16.
//
#include "gl/Error.hpp"
#include "gl/RenderState.hpp"
#include "gl/ShaderProgram.hpp"
#include "gl/VertexLayout.hpp"

namespace stock {

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

void VertexLayout::enable(RenderState& rs, ShaderProgram& program, size_t offset) {

    GLuint glProgram = program.getGlProgram();
    size_t maxVertexAttributes = RenderState::MAX_ATTRIBUTES;

    // Enable all attributes for this layout.
    for (auto& a : m_attributes) {

        GLint location = program.getAttributeLocation(a.name);
        assert(location < maxVertexAttributes);

        if (location >= 0) {
            GLuint& boundProgram = rs.attributeBindings[location];
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
    for (GLuint location = 0; location < maxVertexAttributes; ++location) {

        GLuint& boundProgram = rs.attributeBindings[location];

        if (boundProgram != glProgram && boundProgram != 0) {
            CHECK_GL(glDisableVertexAttribArray(location));
            boundProgram = 0;
        }
    }
}

} // namespace stock
