#define GLFW_INCLUDE_NONE
#include "gl/Error.hpp"
#include "gl/Mesh.hpp"
#include "gl/RenderState.hpp"
#include "gl/ShaderProgram.hpp"
#include "view/Camera.hpp"
#include "ImGuiImpl.hpp"
#include <GLFW/glfw3.h>

using namespace stock;

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

  // Setup ImGui binding
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
  ImGuiImpl::Init(window, true);

  // Setup style
  ImGui::StyleColorsDark();

  ShaderProgram shader(fs_src, vs_src);
  UniformLocation mvpMatrixLocation("u_mvp");

  Mesh<Vertex> mesh;
  mesh.setVertexLayout(VertexLayout({
      VertexAttribute("a_position", 3, GL_FLOAT, GL_FALSE),
      VertexAttribute("a_color", 4, GL_UNSIGNED_BYTE, GL_TRUE),
  }));
  mesh.vertices = {
      {1.f, 1.f, 1.f, 0xff000000},
      {-1.f, 1.f, -1.f, 0xffff0000},
      {1.f, -1.f, -1.f, 0xff00ff00},
      {-1.f, -1.f, 1.f, 0xff0000ff},
  };
  mesh.indices = {0, 2, 1, 0, 1, 3, 0, 3, 2, 1, 2, 3};

  RenderState rs;
  rs.reset();
  rs.clearColor(0.f, 0.f, 0.f, 1.f);


  Camera camera(1024.f, 768.f, Camera::Options());
  camera.setPosition(0.f, -3.f, 0.f);

  Log::setLevel(Log::Level::VERBOSE);

  // Loop until the user closes the window.
  while (!glfwWindowShouldClose(window)) {

    // Poll for and process events.
    glfwPollEvents();
    ImGuiImpl::NewFrame(rs);

    // Create ImGui interface.
    ImGui::Text("Hello, world!");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // Render here.
    rs.culling(true);
    rs.cullFace(GL_BACK);
    rs.depthTest(true);
    rs.blending(false);
    rs.scissorTest(false);

    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    static const glm::vec3 point = {0.f, 0.f, 0.f}, axis = {0.f, 0.f, 1.f};
    camera.orbit(point, axis, 0.016f);
    camera.lookAt(point);
    camera.update();

    shader.setUniformMatrix4f(rs, mvpMatrixLocation, camera.viewProjectionMatrix());

    mesh.draw(rs, shader);

    // Render ImGui interface.
    ImGui::Render();
    ImGuiImpl::RenderDrawData(rs, ImGui::GetDrawData());

    // Swap front and back buffers.
    glfwSwapBuffers(window);
  }

  shader.dispose(rs);
  mesh.dispose(rs);

  ImGuiImpl::Shutdown(rs);

  glfwTerminate();
  return 0;
}
