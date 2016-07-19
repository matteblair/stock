//
// Created by Matt Blair on 4/2/16.
//
#pragma once
#include "gl/GL.hpp"
#include "gl/VertexAttribute.hpp"
#include <string>
#include <vector>

namespace stock {

class ShaderProgram;
class RenderState;

class VertexLayout {

public:

    VertexLayout(std::vector<VertexAttribute> attributes);

    void enable(RenderState& rs, ShaderProgram& program, size_t offset = 0);

    size_t stride() const { return m_stride; };

    size_t getOffset(const std::string& attributeName) const;

private:

    std::vector<VertexAttribute> m_attributes;
    size_t m_stride = 0;

};

} // namespace stock

