#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

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

    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(window)) {
        // Render here.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for and process events.
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
