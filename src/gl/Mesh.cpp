//
// Created by Matt Blair on 5/26/16.
//
#include "gl/Error.hpp"
#include "gl/Mesh.hpp"
#include "gl/RenderState.hpp"
#include "gl/ShaderProgram.hpp"

#define MAX_INDEX_VALUE 65535 // Maximum value of GLushort

namespace stock {

MeshBase::MeshBase() : m_vertexLayout({}) {}

MeshBase::~MeshBase() {}

void MeshBase::dispose(RenderState& rs) {

  // Deleting a index/array buffer being used ends up setting up the current vertex/index buffer to 0
  // after the driver finishes using it, so force the render state to be 0 for vertex/index buffer.

  if (m_glVertexBuffer) {
    rs.vertexBufferUnset(m_glVertexBuffer);
    CHECK_GL(glDeleteBuffers(1, &m_glVertexBuffer));
  }
  if (m_glIndexBuffer) {
    rs.indexBufferUnset(m_glIndexBuffer);
    CHECK_GL(glDeleteBuffers(1, &m_glIndexBuffer));
  }
}

void MeshBase::setVertexLayout(VertexLayout layout) {
  m_vertexLayout = layout;
}

void MeshBase::setDrawMode(GLenum drawMode) {
  m_drawMode = drawMode;
}

void MeshBase::upload(RenderState& rs) {

  if (m_glVertexData && m_vertexCount > 0) {

    // Generate vertex buffer, if needed.
    if (m_glVertexBuffer == 0) {
      CHECK_GL(glGenBuffers(1, &m_glVertexBuffer));
    }

    // Buffer vertex data.
    rs.vertexBuffer(m_glVertexBuffer);
    size_t vertexBytes = m_vertexCount * m_vertexLayout.stride();
    CHECK_GL(glBufferData(GL_ARRAY_BUFFER, vertexBytes, m_glVertexData, m_hint));
  }

  if (m_glIndexData && m_indexCount > 0) {

    // Generate index buffer, if needed.
    if (m_glIndexBuffer == 0) {
      CHECK_GL(glGenBuffers(1, &m_glIndexBuffer));
    }

    // Buffer index data.
    rs.indexBuffer(m_glIndexBuffer);
    size_t indexBytes = m_indexCount * sizeof(GLushort);
    CHECK_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBytes, m_glIndexData, m_hint));
  }

  m_isUploaded = true;
}

bool MeshBase::draw(RenderState& rs, ShaderProgram& shader) {

  // Ensure that geometry is buffered into GPU.
  if (!m_isUploaded) {
    upload(rs);
  }

  // Exit early for empty meshes.
  if (m_vertexCount == 0) {
    return false;
  }

  // Enable the shader program.
  if (!shader.use(rs)) {
    return false;
  }

  // Bind buffers for drawing.
  rs.vertexBuffer(m_glVertexBuffer);

  if (m_indexCount > 0) {
    rs.indexBuffer(m_glIndexBuffer);
  }

  // Enable vertex layout.
  m_vertexLayout.enable(rs, shader);

  // Draw as elements or arrays.
  if (m_indexCount > 0) {
    CHECK_GL(glDrawElements(m_drawMode, m_indexCount, GL_UNSIGNED_SHORT, 0));
  } else if (m_vertexCount > 0) {
    CHECK_GL(glDrawArrays(m_drawMode, 0, m_vertexCount));
  }

  return true;
}

size_t MeshBase::getTotalBufferSize() const {
  return m_vertexCount * m_vertexLayout.stride() + m_indexCount * sizeof(GLushort);
}

} // namespace stock
