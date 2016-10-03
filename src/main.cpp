#define GLFW_INCLUDE_NONE
#include "gl/Error.hpp"
#include "gl/Framebuffer.hpp"
#include "gl/Mesh.hpp"
#include "gl/RenderState.hpp"
#include "gl/ShaderProgram.hpp"
#include "view/Camera.hpp"
#include <GLFW/glfw3.h>

using namespace stock;

const std::string vs_mesh = R"SHADER_END(
attribute vec3 a_position;
attribute vec4 a_color;
varying vec4 v_color;
uniform mat4 u_mvp;
void main() {
  v_color = a_color;
  gl_Position = u_mvp * vec4(a_position, 1.);
}
)SHADER_END";

const std::string fs_mesh = R"SHADER_END(
varying vec4 v_color;
void main() {
  gl_FragColor = v_color;
}
)SHADER_END";

const std::string vs_quad = R"SHADER_END(
attribute vec2 a_position;
varying vec2 v_texcoords;
void main() {
  v_texcoords = (a_position + 1.) / 2.;
  gl_Position = vec4(a_position, 0., 1.);
}
)SHADER_END";

const std::string fs_quad = R"SHADER_END(
uniform sampler2D u_tex;
varying vec2 v_texcoords;
float unpack(vec4 color) {
  const vec4 bitUnshift = vec4(1./(256.*256.*256.), 1./(256.*256.), 1./256., 1.);
  return dot(color, bitUnshift);
}
void main() {
  vec4 color = texture2D(u_tex, v_texcoords);
  float depth = unpack(color);
  gl_FragColor = 5. * vec4(1. - depth);
}
)SHADER_END";

const std::string vs_depth = R"SHADER_END(
attribute vec3 a_position;
attribute vec4 a_color;
varying vec4 v_position;
uniform mat4 u_mvp;
void main() {
  v_position = u_mvp * vec4(a_position, 1.);
  gl_Position = v_position;
}
)SHADER_END";

const std::string fs_depth = R"SHADER_END(
varying vec4 v_position;
vec4 pack(float depth) {
  const vec4 bitShift = vec4(256.*256.*256., 256.*256., 256., 1.);
  const vec4 bitMask = vec4(0., 1./256., 1./256., 1./256.);
  vec4 comp = fract(depth * bitShift);
  comp -= comp.xxyz * bitMask;
  return comp;
}
void main() {
  // Normalize depth.
  float depth = v_position.z / v_position.w;
  // Change range from [-1.0, 1.0] to [0.0, 1.0].
  depth = (depth + 1.0) / 2.0;
  // Pack into 32-bit RGBA texture.
  gl_FragColor = pack(depth);
}
)SHADER_END";

struct Vertex {
  float x, y, z;
  unsigned int color;
};

int main(void) {

  GLFWwindow* window;

  // Initialize the library.
  if (!glfwInit()) {
    return -1;
  }

  // Create a windowed mode window and its OpenGL context.
  window = glfwCreateWindow(1024, 768, "GLFW Window", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  // Make the window's context current.
  glfwMakeContextCurrent(window);

  gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress);

  ShaderProgram meshShader(fs_mesh, vs_mesh);
  UniformLocation meshMvpLocation("u_mvp");

  Mesh<Vertex> mesh;
  mesh.setVertexLayout(VertexLayout({
      VertexAttribute("a_position", 3, GL_FLOAT, GL_FALSE), VertexAttribute("a_color", 4, GL_UNSIGNED_BYTE, GL_TRUE),
  }));
  mesh.vertices = {
      { 1.f,  1.f,  1.f, 0xff000000},
      {-1.f,  1.f, -1.f, 0xffff0000},
      { 1.f, -1.f, -1.f, 0xff00ff00},
      {-1.f, -1.f,  1.f, 0xff0000ff},
  };
  mesh.indices = {0, 1, 2, 0, 3, 1, 0, 2, 3, 1, 3, 2};

  ShaderProgram quadShader(fs_quad, vs_quad);
  UniformLocation quadTexLocation("u_tex");

  Mesh<glm::vec2> quad;
  quad.setVertexLayout(VertexLayout({VertexAttribute("a_position", 2, GL_FLOAT, GL_FALSE)}));
  quad.vertices = {{-1.f,  1.f}, {-1.f, -1.f}, { 1.f, -1.f}, { 1.f,  1.f}};
  quad.indices = {0, 1, 2, 2, 3, 0};

  ShaderProgram depthShader(fs_depth, vs_depth);
  UniformLocation depthMvpLocation("u_mvp");

  Framebuffer::Options depthFramebufferOptions;
  depthFramebufferOptions.hasDepth = true;
  Framebuffer depthFramebuffer(1024, 768, depthFramebufferOptions);

  RenderState rs;
  rs.configure();
  rs.clearColor(0.f, 0.f, 0.f, 1.f);
  rs.depthTest(GL_TRUE);

  Camera camera(1024.f, 768.f, Camera::Options());
  camera.setPosition(0.f, -3.f, 0.f);

  Log::setLevel(Log::Level::VERBOSE);

  // Loop until the user closes the window.
  while (!glfwWindowShouldClose(window)) {
    // Render here.
    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    static const glm::vec3 point = {0.f, 0.f, 0.f}, axis = {0.f, 0.f, 1.f};
    camera.orbit(point, axis, 0.016f);
    camera.lookAt(point);
    camera.update();

    // Draw the depth into a texture.
    depthFramebuffer.bind(rs);
    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    depthShader.setUniformMatrix4f(rs, depthMvpLocation, camera.viewProjectionMatrix());
    mesh.draw(rs, depthShader);
    depthFramebuffer.unbind(rs);

    // Draw the depth texture to the screen.
    depthFramebuffer.colorTexture().bind(rs);
    quadShader.setUniformi(rs, quadTexLocation, rs.currentTextureUnit());
    quad.draw(rs, quadShader);

    // Swap front and back buffers.
    glfwSwapBuffers(window);

    // Poll for and process events.
    glfwPollEvents();
  }

  meshShader.dispose(rs);
  mesh.dispose(rs);
  quadShader.dispose(rs);
  quad.dispose(rs);
  depthShader.dispose(rs);
  depthFramebuffer.dispose(rs);

  glfwTerminate();
  return 0;
}
