//
// Created by Matt Blair on 7/10/16.
//

#pragma once
#include "gl/GL.hpp"
#include <string>

namespace stock {

struct VertexAttribute {

    VertexAttribute(std::string name, GLint size, GLenum type, GLboolean normalized) :
        name(name), offset(0), size(size), type(type), normalized(normalized) {}

    std::string name;
    size_t offset;
    GLint size;
    GLenum type;
    GLboolean normalized;

};

} // namespace stock
