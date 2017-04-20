#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "app/App.hpp"

using namespace stock;

int main(void) {

  GLFWwindow* window = nullptr;

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

  // Load GL functions.
  gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress);

  // Create the App.
  App app;
  WindowListener* listener = &app;

  listener->create();
  listener->resize(1024, 768);

  // Loop until the user closes the window.
  while (!glfwWindowShouldClose(window)) {
    // Render here.
    listener->render();

    // Swap front and back buffers.
    glfwSwapBuffers(window);

    // Poll for and process events.
    glfwPollEvents();
  }

  listener->dispose();

  glfwTerminate();
  return 0;
}
