//
// Created by Matt Blair on 4/2/16.
//
#include "VertexLayout.hpp"
#include "ShaderProgram.hpp"
#include "Error.hpp"

namespace stock {

std::map<GLint, GLuint> VertexLayout::s_enabledAttribs;

VertexLayout::VertexLayout(std::vector<VertexAttrib> attribs) : m_attribs(attribs) {

    m_stride = 0;

    for (auto& attrib : m_attribs) {

        // Set the offset of this vertex attribute: The stride at this point
        // denotes the number of bytes into the vertex by which this attribute
        // is offset, but we must cast the number as a void* to use with
        // glVertexAttribPointer; We use reinterpret_cast to avoid warnings.
        attrib.offset = m_stride;

        GLint byteSize = attrib.size;

        switch (attrib.type) {
            case GL_FLOAT:
            case GL_INT:
            case GL_UNSIGNED_INT:
                byteSize *= 4; // 4 bytes for floats, ints, and uints
                break;
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
                byteSize *= 2; // 2 bytes for shorts and ushorts
                break;
            default:
                break;
        }

        m_stride += byteSize;

    }
}

size_t VertexLayout::getOffset(const std::string& attribName) {

    for (auto& attrib : m_attribs) {
        if (attrib.name == attribName) {
            return attrib.offset;
        }
    }

    // LOGE("No such attribute %s", attribName.c_str());
    return 0;
}

void VertexLayout::enable(const std::map<std::string, GLuint>& locations, size_t byteOffset) {

    for (auto& attrib : m_attribs) {
        auto it = locations.find(attrib.name);

        if (it == locations.end()) {
            continue;
        }

        GLint location = it->second;;

        if (location != -1) {
            void* offset = ((unsigned char*) attrib.offset) + byteOffset;
            CHECK_GL(glEnableVertexAttribArray(location));
            CHECK_GL(glVertexAttribPointer(location, attrib.size, attrib.type, attrib.normalized, m_stride, offset));
        }
    }

}

void VertexLayout::clearCache() {
    s_enabledAttribs.clear();
}

void VertexLayout::enable(ShaderProgram& program, size_t byteOffset, void* ptr) {

    GLuint glProgram = program.getGlProgram();

    // Enable all attributes for this layout.
    for (auto& attrib : m_attribs) {

        GLint location = program.getAttribLocation(attrib.name);

        if (location != -1) {
            auto& loc = s_enabledAttribs[location];
            // Track currently enabled attributess by the program to which they are bound.
            if (loc != glProgram) {
                CHECK_GL(glEnableVertexAttribArray(location));
                loc = glProgram;
            }

            void* data = (unsigned char*)ptr + attrib.offset + byteOffset;
            CHECK_GL(glVertexAttribPointer(location, attrib.size, attrib.type, attrib.normalized, m_stride, data));
        }
    }

    // Disable previously bound and now-unneeded attributes
    for (auto& locationProgramPair : s_enabledAttribs) {

        const GLint& location = locationProgramPair.first;
        GLuint& boundProgram = locationProgramPair.second;

        if (boundProgram != glProgram && boundProgram != 0) {
            CHECK_GL(glDisableVertexAttribArray(location));
            boundProgram = 0;
        }
    }
}

} // namespace stock
