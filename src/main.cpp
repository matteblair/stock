#define GLFW_INCLUDE_NONE
#include "debug/DebugDraw.hpp"
#include "gl/Error.hpp"
#include "gl/Mesh.hpp"
#include "gl/RenderState.hpp"
#include "gl/ShaderProgram.hpp"
#include "io/UrlSession.hpp"
#include "view/Camera.hpp"
#include "view/ViewOrbit.hpp"
#include "terrain/TerrainRenderer.hpp"
#include "terrain/TileView.hpp"
#include "ImGuiImpl.hpp"
#include <GLFW/glfw3.h>

using namespace stock;

struct AppData {
  RenderState rs;
  TerrainRenderer model;
  TileView view;
  ViewOrbit orbit;
  std::vector<TerrainData> terrainTiles;
};

void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
  AppData* app = reinterpret_cast<AppData*>(glfwGetWindowUserPointer(window));

  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    if (key == GLFW_KEY_G) {
      app->model.toggleGrid();
    } else if (key == GLFW_KEY_H) {
      app->model.toggleHull();
    } else if (key == GLFW_KEY_R) {
      app->orbit.reset();
    } else if (key == GLFW_KEY_UP) {
      Log::df("Resolution increased to: %d\n", app->model.increaseResolution());
    } else if (key == GLFW_KEY_DOWN) {
      Log::df("Resolution decreased to: %d\n", app->model.decreaseResolution());
    }
  }
}

void onMouseMove(GLFWwindow* window, double xpos, double ypos) {
  AppData* app = reinterpret_cast<AppData*>(glfwGetWindowUserPointer(window));
  bool mouseDown = static_cast<bool>(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT));
  float xNorm = static_cast<float>(xpos) / app->view.camera().width();
  float yNorm = static_cast<float>(ypos) / app->view.camera().height();
  //app->orbit.drag(xNorm, yNorm, mouseDown);
}

void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
  AppData* app = reinterpret_cast<AppData*>(glfwGetWindowUserPointer(window));
  app->view.setZoom(app->view.zoom() + yoffset / 100.);
}

int main(void) {

  // Create the application data container.
  AppData app{};

  // Initialize GLFW.
  if (!glfwInit()) {
    return -1;
  }

  // Set anti-aliasing on.
  glfwWindowHint(GLFW_SAMPLES, 4);

  // Create a windowed mode window and its OpenGL context.
  int windowWidth = static_cast<int>(app.view.camera().width());
  int windowHeight = static_cast<int>(app.view.camera().height());
  GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "GLFW Window", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  // Make the window's context current.
  glfwMakeContextCurrent(window);

  // Load OpenGL functions with GLAD.
  gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress);

  Log::setLevel(Log::Level::DEBUGGING);

  // Setup ImGui binding
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
  ImGuiImpl::Init(window, true);

  // Setup style
  ImGui::StyleColorsDark();

  app.rs.reset();
  app.rs.clearColor(0.f, 0.f, 0.f, 1.f);
  app.rs.depthTest(GL_TRUE);
  app.rs.culling(GL_TRUE);

  app.model.generateMesh(64);

  app.view.camera().transform().setDirection({0.f, 0.f, -1.f});
  app.view.camera().setUpVector({0.f, 1.f, 0.f});
  app.view.setPosition(LngLat(-121.9320666, 37.8719037));
  app.view.setZoom(14);
  app.view.update();

  auto tiles = app.view.visibleTiles();

  UrlSession urlSession({});

  std::string elevationDataUrlTemplate = "https://tile.nextzen.org/tilezen/terrain/v1/256/terrarium/%d/%d/%d.png?api_key=";
  std::string normalDataUrlTemplate = "https://tile.nextzen.org/tilezen/terrain/v1/256/normal/%d/%d/%d.png?api_key=";
  if (const char* apiKeyEnvVar = getenv("NEXTZEN_API_KEY")) {
    elevationDataUrlTemplate += apiKeyEnvVar;
    normalDataUrlTemplate += apiKeyEnvVar;
  }

  app.terrainTiles.reserve(tiles.size()); // Can't reallocate in this loop!
  for (const auto& tile : tiles) {
    app.terrainTiles.emplace_back(tile);
    auto& terrainData = app.terrainTiles.back();
    auto elevationDataUrl = terrainData.populateUrlTemplate(elevationDataUrlTemplate);
    urlSession.addRequest(elevationDataUrl, [&](UrlSession::Response& response) {
      Log::vf("Received elevation URL response! Data length: %d\n", response.data.size());
      terrainData.loadElevationData(response.data);
    });
    auto normalDataUrl = terrainData.populateUrlTemplate(normalDataUrlTemplate);
    urlSession.addRequest(normalDataUrl, [&](UrlSession::Response& response) {
      Log::vf("Received normal URL response! Data length: %d\n", response.data.size());
      terrainData.loadNormalData(response.data);
    });
  }

  glfwSetWindowUserPointer(window, &app);
  glfwSetKeyCallback(window, &onKeyEvent);
  glfwSetCursorPosCallback(window, &onMouseMove);
  glfwSetScrollCallback(window, &onMouseScroll);

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

    //app.view.camera().transform().setDirection(app.orbit.getDirection());
    app.view.update();

    for (auto& tile : app.terrainTiles) {
      app.model.render(app.rs, tile, app.view);
    }

    static const glm::vec3 point = {0.f, 0.f, -1.f};
    glm::dvec2 lastMousePosition = mousePosition;
    glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
    auto mouseDelta = mousePosition - lastMousePosition;
    auto mouseDistance = glm::length(mouseDelta);
    auto mouseScreenVector = (float)mouseDelta.x * Transform::RIGHT - (float)mouseDelta.y * Transform::UP;
    auto mouseWorldVector = app.view.camera().transform().convertLocalVectorToWorld(mouseScreenVector);
    auto orbitAxis = glm::cross(app.view.camera().transform().getDirection(), mouseWorldVector);
    if (glfwGetMouseButton(window, 0)) {
      if (glm::abs(mouseDistance) > 0.0001) {
        app.view.camera().transform().orbit(point, orbitAxis, mouseDistance * 0.01);
        app.view.camera().lookAt(point);
        auto up = app.view.camera().transform().convertLocalVectorToWorld(Transform::UP);
        app.view.camera().setUpVector(up);
      }
    }

    // ImGui::ShowDemoWindow();

    if (ImGui::TreeNode("Camera Transform")) {
      auto position = app.view.camera().transform().position();
      auto direction = app.view.camera().transform().getDirection();
      ImGui::InputFloat3("Position", &position.x, 4);
      ImGui::InputFloat3("Direction", &direction.x, 4);
      ImGui::TreePop();
    }

    DebugDraw::cameraMatrix(app.view.camera().viewProjectionMatrix());
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

  app.model.dispose(app.rs);

  for (auto& tile : app.terrainTiles) {
    tile.dispose(app.rs);
  }

  DebugDraw::dispose(app.rs);

  ImGuiImpl::Shutdown(app.rs);

  glfwTerminate();
  return 0;
}
