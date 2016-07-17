#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <gl/RenderState.hpp>
#include "gl/ShaderProgram.hpp"
#include "gl/Mesh.hpp"

using namespace stock;

const std::string vs_src = R"SHADER_END(
attribute vec2 a_position;
attribute vec4 a_color;
varying vec4 v_color;
void main() {
    v_color = a_color;
    gl_Position = vec4(a_position, 1., 1.);
}
)SHADER_END";

const std::string fs_src = R"SHADER_END(
varying vec4 v_color;
void main() {
    gl_FragColor = v_color;
}
)SHADER_END";

struct Vertex {
    float x, y;
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

    glClearColor(0.f, 0.f, 0.f, 1.f);

    ShaderProgram shader;
    shader.setSourceStrings(fs_src, vs_src);

    Mesh<Vertex> mesh;
    mesh.setVertexLayout(VertexLayout(
        {
            VertexAttribute("a_position", 2, GL_FLOAT, false),
            VertexAttribute("a_color", 4, GL_UNSIGNED_BYTE, true),
        }
    ));
    mesh.vertices = {
        { 1.f, 0.f, 0xffff0000 },
        { .5f, 1.f, 0xff00ff00 },
        { 0.f, 0.f, 0xff0000ff },
    };

    RenderState rs;
    rs.configure();

    Log::setLevel(stock::Log::Level::verbose);

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(window)) {
        // Render here.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mesh.draw(rs, shader);

        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for and process events.
        glfwPollEvents();
    }

    shader.dispose(rs);
    mesh.dispose(rs);

    glfwTerminate();
    return 0;
}
