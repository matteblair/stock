//
// Created by Matt Blair on 2/18/17.
//

#include "app/App.hpp"
#include "gl/Error.hpp"
#include "io/Log.hpp"

namespace stock {

const std::string vs_src = R"SHADER_END(
attribute vec3 a_position;
attribute vec4 a_color;
varying vec4 v_color;
uniform mat4 u_mvp;
void main() {
    v_color = a_color;
    gl_Position = u_mvp * vec4(a_position, 1.);
}
)SHADER_END";

const std::string fs_src = R"SHADER_END(
varying vec4 v_color;
void main() {
    gl_FragColor = v_color;
}
)SHADER_END";

App::App() :
  shader(fs_src, vs_src),
  mvpMatrixLocation("u_mvp"),
  camera(1.f, 1.f, Camera::Options{}) {}

void App::create() {
  // Set up Mesh.
  mesh.setVertexLayout(VertexLayout({
      VertexAttribute("a_position", 3, GL_FLOAT, GL_FALSE), VertexAttribute("a_color", 4, GL_UNSIGNED_BYTE, GL_TRUE),
  }));
  mesh.vertices = {
      {1.f, 1.f, 1.f, 0xff000000},
      {-1.f, 1.f, -1.f, 0xffff0000},
      {1.f, -1.f, -1.f, 0xff00ff00},
      {-1.f, -1.f, 1.f, 0xff0000ff},
  };
  mesh.indices = {0, 1, 2, 0, 3, 1, 0, 2, 3, 1, 3, 2};

  // Set up RenderState.
  rs.reset();
  rs.clearColor(0.f, 0.f, 0.f, 1.f);
  rs.depthTest(GL_TRUE);

  // Set up Camera.
  camera.setPosition(0.f, -3.f, 0.f);

  // Set up Log.
  Log::setLevel(Log::Level::VERBOSE);
}

void App::resize(uint32_t width, uint32_t height) {
  // Resize Camera.
  camera.resize(static_cast<float>(width), static_cast<float>(height));
}

void App::render() {
  CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  static const glm::vec3 point = {0.f, 0.f, 0.f}, axis = {0.f, 0.f, 1.f};
  camera.orbit(point, axis, 0.016f);
  camera.lookAt(point);
  camera.update();

  shader.setUniformMatrix4f(rs, mvpMatrixLocation, camera.viewProjectionMatrix());

  mesh.draw(rs, shader);
}

void App::pause() {
  // Nothing to do.
}

void App::resume() {
  // Nothing to do.
}

void App::dispose() {
  shader.dispose(rs);
  mesh.dispose(rs);
}

}
