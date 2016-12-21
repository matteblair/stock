#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "gl/Error.hpp"
#include "gl/Mesh.hpp"
#include "gl/RenderState.hpp"
#include "gl/ShaderProgram.hpp"
#include "io/File.hpp"
#include "view/Camera.hpp"
#include "terrain/TerrainModel.hpp"

using namespace stock;

struct AppData {
  RenderState rs;
  Camera camera = Camera(1024.f, 768.f, Camera::Options());
  TerrainModel terrain;
  float orbit = 0.f;
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
    case GLFW_KEY_LEFT:
      app->orbit = 0.01f;
      break;
    case GLFW_KEY_RIGHT:
      app->orbit = -0.01f;
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

  AppData app;
  app.rs.configure();
  app.rs.clearColor(0.f, 0.f, 0.f, 1.f);
  app.rs.depthTest(GL_TRUE);
  app.rs.culling(GL_TRUE);

  app.camera.setPosition(0.f, -.1f, .6f);

  app.terrain.loadElevationTexture(File("elevation.png").readAll());
  app.terrain.loadNormalTexture(File("normals.png").readAll());
  app.terrain.generateMesh(64);

  Log::setLevel(Log::Level::VERBOSE);

  glfwSetWindowUserPointer(window, &app);

  glfwSetKeyCallback(window, &onKeyEvent);

  // Loop until the user closes the window.
  while (!glfwWindowShouldClose(window)) {
    // Render here.
    CHECK_GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    static const glm::vec3 point = {.5f, .5f, 0.f}, axis = {0.f, 0.f, 1.f};
    app.camera.orbit(point, axis, app.orbit);
    app.camera.lookAt(point);
    app.camera.update();
    app.orbit *= 0.95;

    app.terrain.render(app.rs, app.camera);

    // Swap front and back buffers.
    glfwSwapBuffers(window);

    // Poll for and process events.
    glfwPollEvents();
  }

  app.terrain.dispose(app.rs);

  glfwTerminate();
  return 0;
}
