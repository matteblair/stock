//
// Created by Matt Blair on 4/2/16.
//
#pragma once
#include "gl/GL.hpp"
#include "gl/VertexAttribute.hpp"
#include <map>
#include <string>
#include <vector>

namespace stock {

class ShaderProgram;

class VertexLayout {

public:

    VertexLayout(std::vector<VertexAttribute> attributes);

    void enable(ShaderProgram& program, size_t offset = 0);

    size_t stride() const { return m_stride; };

    size_t getOffset(const std::string& attributeName) const;

    static void clearCache();

private:

    static std::map<GLuint, GLuint> s_enabledAttributes; // Map from attribute locations to bound shader program

    std::vector<VertexAttribute> m_attributes;
    size_t m_stride = 0;

};

} // namespace stock

