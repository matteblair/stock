//
// Created by Matt Blair on 5/26/16.
//
#include "gl/Mesh.hpp"
#include "gl/ShaderProgram.hpp"
#include "gl/RenderState.hpp"

#define MAX_INDEX_VALUE 65535 // Maximum value of GLushort

namespace stock {

MeshBase::MeshBase() {
}

MeshBase::~MeshBase() {
}

void MeshBase::dispose() {

    // Deleting a index/array buffer being used ends up setting up the current vertex/index buffer to 0
    // after the driver finishes using it, so force the render state to be 0 for vertex/index buffer.

    if (m_glVertexBuffer) {
        if (RenderState::vertexBuffer.compare(m_glVertexBuffer)) {
            RenderState::vertexBuffer.init(0, false);
        }
        CHECK_GL(glDeleteBuffers(1, &m_glVertexBuffer));
    }
    if (m_glIndexBuffer) {
        if (RenderState::indexBuffer.compare(m_glIndexBuffer)) {
            RenderState::indexBuffer.init(0, false);
        }
        CHECK_GL(glDeleteBuffers(1, &m_glIndexBuffer));
    }
}

void MeshBase::setVertexLayout(VertexLayout layout) {
    m_vertexLayout = layout;
}

void MeshBase::setDrawMode(GLenum drawMode) {
    m_drawMode = drawMode;
}

void MeshBase::upload() {

    if (m_glVertexData && m_vertexCount > 0) {

        // Generate vertex buffer, if needed.
        if (m_glVertexBuffer == 0) {
            CHECK_GL(glGenBuffers(1, &m_glVertexBuffer));
        }

        // Buffer vertex data.
        RenderState::vertexBuffer(m_glVertexBuffer);
        size_t vertexBytes = m_vertexCount * m_vertexLayout.stride();
        CHECK_GL(glBufferData(GL_ARRAY_BUFFER, vertexBytes, m_glVertexData, m_hint));

    }

    if (m_glIndexData && m_indexCount > 0) {

        // Generate index buffer, if needed.
        if (m_glIndexBuffer == 0) {
            CHECK_GL(glGenBuffers(1, &m_glIndexBuffer));
        }

        // Buffer index data.
        RenderState::indexBuffer(m_glIndexBuffer);
        size_t indexBytes = m_indexCount * sizeof(GLushort);
        CHECK_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBytes, m_glIndexData, m_hint));

    }

    m_generation = RenderState::generation();

    m_isUploaded = true;
}

bool MeshBase::draw(ShaderProgram& shader) {

    checkValidity();

    // Ensure that geometry is buffered into GPU.
    if (!m_isUploaded) { upload(); }

    // Exit early for empty meshes.
    if (m_vertexCount == 0) { return false; }

    // Enable the shader program.
    if (!shader.use()) { return false; }

    // Bind buffers for drawing.
    RenderState::vertexBuffer(m_glVertexBuffer);

    if (m_indexCount > 0) {
        RenderState::indexBuffer(m_glIndexBuffer);
    }

    // Enable vertex layout.
    m_vertexLayout.enable(shader, 0);

    // Draw as elements or arrays.
    if (m_indexCount > 0) {
        CHECK_GL(glDrawElements(m_drawMode, m_indexCount, GL_UNSIGNED_SHORT, 0));
    } else if (m_vertexCount > 0) {
        CHECK_GL(glDrawArrays(m_drawMode, 0, m_vertexCount));
    }

    return true;
}

bool MeshBase::checkValidity() {

    if (!RenderState::isValidGeneration(m_generation)) {
        m_isUploaded = false;
        m_glVertexBuffer = 0;
        m_glIndexBuffer = 0;
        m_generation = RenderState::generation();
        return false;
    }

    return true;
}

size_t MeshBase::getTotalBufferSize() const {
    return m_vertexCount * m_vertexLayout.stride() + m_indexCount * sizeof(GLushort);
}

} // namespace stock
