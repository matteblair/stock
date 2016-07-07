//
// Created by Matt Blair on 5/26/16.
//
#pragma once

#include "GL.hpp"
#include "VertexLayout.hpp"
#include <string>
#include <vector>
#include <memory>

namespace stock {

// Mesh is a drawable collection of geometry contained in a vertex buffer and
// (optionally) an index buffer.

class MeshBase {

public:

    // Create an empty mesh with default options.
    MeshBase();

    virtual ~MeshBase();

    // Set the vertex layout to use when binding this Mesh for drawing.
    void setVertexLayout(VertexLayout layout);

    // Set the GL draw mode for this Mesh.
    void setDrawMode(GLenum mode);

    // Copy all added vertices and indices into OpenGL buffer objects; After
    // geometry is uploaded, no more vertices or indices can be added.
    virtual void upload();

    // Release all OpenGL resources for this Mesh.
    void dispose();

    // Render the geometry in this mesh using the ShaderProgram _shader; if
    // geometry has not already been uploaded it will be uploaded at this point.
    bool draw(ShaderProgram& _shader);

    // Get the total size of VRAM in bytes used by this Mesh.
    size_t getTotalBufferSize() const;

protected:

    bool checkValidity();

    VertexLayout m_vertexLayout;

    // Generation in which this mesh's GL handles were created
    int m_generation = -1;

    // Vertex data
    size_t m_vertexCount = 0;
    GLuint m_glVertexBuffer = 0;
    GLbyte* m_glVertexData = nullptr;

    // Index data
    size_t m_indexCount = 0;
    GLuint m_glIndexBuffer = 0;
    GLushort* m_glIndexData = nullptr;

    GLenum m_drawMode = GL_TRIANGLES;
    GLenum m_hint = GL_STATIC_DRAW;

    bool m_isUploaded = false;

};

template<class T>
class Mesh : public MeshBase {

public:
    std::vector<T> vertices;
    std::vector<uint16_t> indices;
    bool retainData = false;

    virtual void upload();
    void reset();
};

template<class T>
void Mesh<T>::reset() {
    vertices.clear();
    indices.clear();
    m_isUploaded = false;
}

template<class T>
void Mesh<T>::upload() {
    m_glVertexData = reinterpret_cast<GLbyte*>(vertices.data());
    m_vertexCount = vertices.size();
    m_glIndexData = indices.data();
    m_indexCount = indices.size();
    MeshBase::upload();
    if (!retainData) {
        vertices.clear();
        indices.clear();
    }
}

// TODO: To support indexed meshes of more than 2^16 vertices, we'll need to
// use 32-bit integers in Mesh::indices and then compile them into 1 or more
// subsets of 16-bit indices, which will each constitue a separate draw call.

// TODO: Add support for Vertex Array Objects

} // namespace stock

