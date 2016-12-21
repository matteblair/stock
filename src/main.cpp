#define GLFW_INCLUDE_NONE
#include "debug/DebugDraw.hpp"
#include "gl/Error.hpp"
#include "gl/Mesh.hpp"
#include "gl/RenderState.hpp"
#include "gl/ShaderProgram.hpp"
#include "io/File.hpp"
#include "io/UrlSession.hpp"
#include "view/Camera.hpp"
#include "terrain/TerrainModel.hpp"
#include "ImGuiImpl.hpp"
#include <GLFW/glfw3.h>

using namespace stock;

struct AppData {
  RenderState rs;
  Camera camera = Camera(1024.f, 768.f, Camera::Options());
  TerrainModel terrain;
};

void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  AppData* app = reinterpret_cast<AppData*>(glfwGetWindowUserPointer(window));

  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch (key) {
    case GLFW_KEY_G:
      app->terrain.toggleGrid();
      break;
    case GLFW_KEY_H:
      app->terrain.toggleHull();
      break;
    case GLFW_KEY_UP:
      Log::df("Resolution increased to: %d\n", app->terrain.increaseResolution());
      break;
    case GLFW_KEY_DOWN:
      Log::df("Resolution decreased to: %d\n", app->terrain.decreaseResolution());
      break;
    }
  }
}

int main(void) {

  GLFWwindow* window;

  // Initialize the library.
  if (!glfwInit()) {
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);

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

  AppData app;
  app.rs.reset();
  app.rs.clearColor(0.f, 0.f, 0.f, 1.f);
  app.rs.depthTest(GL_TRUE);
  app.rs.culling(GL_TRUE);

  app.camera.transform().position() = {0.f, -.1f, .6f};

  app.terrain.loadElevationTexture(File("elevation.png").readAll());
  app.terrain.loadNormalTexture(File("normals.png").readAll());
  app.terrain.generateMesh(64);

  Log::setLevel(Log::Level::DEBUGGING);

  glfwSetWindowUserPointer(window, &app);

  glfwSetKeyCallback(window, &onKeyEvent);

  UrlSession urlSession({});
  urlSession.addRequest("http://vector.mapzen.com/osm/all/16/17583/24208.json", [](UrlSession::Response response) {
    Log::df("Received URL response! Data length: %d\n", response.data.size());
  });

  glm::dvec2 mousePosition;

  // Loop until the user closes the window.
  while (!glfwWindowShouldClose(window)) {

    // Poll for and process events.
    glfwPollEvents();
    ImGuiImpl::NewFrame(app.rs);

    // Create ImGui interface.
    ImGui::Text("Hello, world!");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // Render here.
    app.rs.culling(true);
    app.rs.cullFace(GL_BACK);
    app.rs.depthTest(true);
    app.rs.blending(false);
    app.rs.scissorTest(false);

    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    static const glm::vec3 point = {.5f, .5f, 0.f}, axis = {0.f, 0.f, 1.f};

    app.terrain.render(app.rs, app.camera);

    glm::dvec2 lastMousePosition = mousePosition;
    glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
    auto mouseDelta = mousePosition - lastMousePosition;
    auto mouseDistance = glm::length(mouseDelta);
    auto mouseScreenVector = (float)mouseDelta.x * Transform::RIGHT - (float)mouseDelta.y * Transform::UP;
    auto mouseWorldVector = app.camera.transform().convertLocalVectorToWorld(mouseScreenVector);
    auto orbitAxis = glm::cross(app.camera.transform().getDirection(), mouseWorldVector);
    if (glfwGetMouseButton(window, 0)) {
      if (glm::abs(mouseDistance) > 0.0001) {
        app.camera.transform().orbit(point, orbitAxis, mouseDistance * 0.01);
        app.camera.lookAt(point);
      }
    }

    // ImGui::ShowDemoWindow();

    if (ImGui::TreeNode("Camera Transform")) {
      auto position = app.camera.transform().position();
      auto direction = app.camera.transform().getDirection();
      ImGui::InputFloat3("Position", &position.x, 4);
      ImGui::InputFloat3("Direction", &direction.x, 4);
      ImGui::TreePop();
    }

    DebugDraw::cameraMatrix(app.camera.viewProjectionMatrix());
    DebugDraw::point(app.rs, {1.f, 0.f, 0.f});
    DebugDraw::point(app.rs, {0.f, 1.f, 0.f});
    DebugDraw::point(app.rs, {0.f, 0.f, 1.f});

    DebugDraw::linestring(app.rs, {
      { 1.f, -1.f, -1.f},
      { 1.f,  1.f, -1.f},
      {-1.f,  1.f, -1.f},
      {-1.f, -1.f, -1.f},
      { 1.f, -1.f, -1.f},
    });

    // Render ImGui interface.
    ImGui::Render();
    ImGuiImpl::RenderDrawData(app.rs, ImGui::GetDrawData());

    // Swap front and back buffers.
    glfwSwapBuffers(window);
  }

  app.terrain.dispose(app.rs);

  DebugDraw::dispose(app.rs);

  ImGuiImpl::Shutdown(app.rs);

  glfwTerminate();
  return 0;
}
