//
// Created by Matt Blair on 4/2/16.
//
#pragma once
#include "GL.hpp"
#include <vector>
#include <map>
#include <memory>
#include <string>

namespace stock {

class ShaderProgram;

class VertexLayout {

public:

    struct VertexAttrib {
        VertexAttrib(std::string name, GLint size, GLenum type, GLboolean normalized) :
            name(name), size(size), type(type), normalized(normalized), offset(0) {}

        std::string name;
        GLint size;
        GLenum type;
        GLboolean normalized;
        size_t offset;
    };

    VertexLayout() = default;
    VertexLayout(std::vector<VertexAttrib> attribs);

    void enable(ShaderProgram& program, size_t byteOffset, void* ptr = nullptr);

    void enable(const std::map<std::string, GLuint>& locations, size_t byteOffset);

    size_t stride() const { return m_stride; };

    const std::vector<VertexAttrib> attribs() const { return m_attribs; }

    size_t getOffset(const std::string& attribName);

    static void clearCache();

private:

    static std::map<GLint, GLuint> s_enabledAttribs; // Map from attrib locations to bound shader program

    std::vector<VertexAttrib> m_attribs;
    size_t m_stride = 0;

};

} // namespace stock

