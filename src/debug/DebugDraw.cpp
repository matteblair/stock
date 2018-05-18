//
// Created by Matt Blair on 4/3/18.
//

#include "debug/DebugDraw.hpp"
#include "gl/Mesh.hpp"
#include "gl/ShaderProgram.hpp"
#include "gl/ShaderUniform.hpp"
#include "gl/VertexLayout.hpp"

namespace stock {

namespace DebugDraw {

struct PosColVert {
  float x, y, z;
  unsigned int color;
};

// Declare static resources for rendering.
static const std::string vs_src = R"SHADER_END(
attribute vec3 a_position;
attribute vec4 a_color;
varying vec4 v_color;
uniform mat4 u_mvp;
void main() {
    v_color = a_color;
    gl_Position = u_mvp * vec4(a_position, 1.);
}
)SHADER_END";
static const std::string fs_src = R"SHADER_END(
varying vec4 v_color;
void main() {
    gl_FragColor = v_color;
}
)SHADER_END";
constexpr unsigned int X_AXIS_COLOR = 0xff0000ff;
constexpr unsigned int Y_AXIS_COLOR = 0xff00ff00;
constexpr unsigned int Z_AXIS_COLOR = 0xffff0000;
static glm::mat4 g_modelViewProjectionMatrix;
static ShaderProgram g_shaderProgram(fs_src, vs_src);
static UniformLocation g_mvpMatrixLocation("u_mvp");
static Mesh<PosColVert> g_mesh;
static VertexLayout g_vertexLayout({
  VertexAttribute("a_position", 3, GL_FLOAT, GL_FALSE),
  VertexAttribute("a_color", 4, GL_UNSIGNED_BYTE, GL_TRUE),
});

void dispose(RenderState& rs) {
  g_shaderProgram.dispose(rs);
  g_mesh.dispose(rs);
}

void cameraMatrix(const glm::mat4& matrix) {
  g_modelViewProjectionMatrix = matrix;
}

// Render the current mesh contents immediately and then reset it.
void render(RenderState& rs) {
  rs.culling(false);
  rs.blending(false);
  rs.depthTest(false);
  g_shaderProgram.setUniformMatrix4f(rs, g_mvpMatrixLocation, g_modelViewProjectionMatrix);
  g_mesh.setVertexLayout(g_vertexLayout);
  g_mesh.upload(rs, GL_STREAM_DRAW);
  g_mesh.draw(rs, g_shaderProgram);
  g_mesh.reset();
}

void point(RenderState& rs, const glm::vec3& pos) {
  g_mesh.vertices = {
     {pos.x - 0.5f, pos.y, pos.z, X_AXIS_COLOR},
     {pos.x + 0.5f, pos.y, pos.z, X_AXIS_COLOR},
     {pos.x, pos.y - 0.5f, pos.z, Y_AXIS_COLOR},
     {pos.x, pos.y + 0.5f, pos.z, Y_AXIS_COLOR},
     {pos.x, pos.y, pos.z - 0.5f, Z_AXIS_COLOR},
     {pos.x, pos.y, pos.z + 0.5f, Z_AXIS_COLOR},
  };
  g_mesh.setDrawMode(GL_LINES);
  render(rs);
}

void linestring(RenderState& rs, const std::vector<glm::vec3>& positions) {
  g_mesh.vertices.reserve(positions.size());
  for (const auto& pos : positions) {
    g_mesh.vertices.push_back({pos.x, pos.y, pos.z, 0xff00ffff});
  }
  g_mesh.setDrawMode(GL_LINE_STRIP);
  render(rs);
}

} // namespace DebugDraw

} // namespace stock
