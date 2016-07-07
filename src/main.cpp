#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <gl/RenderState.hpp>
#include "gl/ShaderProgram.hpp"
#include "gl/Mesh.hpp"

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

    stock::ShaderProgram shader;
    shader.setSourceStrings(fs_src, vs_src);

    stock::VertexLayout vertexLayout(
        {
            stock::VertexLayout::VertexAttrib{"a_position", 2, GL_FLOAT, false},
            stock::VertexLayout::VertexAttrib{"a_color", 4, GL_UNSIGNED_BYTE, true},
        }
    );

    stock::Mesh<Vertex> mesh;
    mesh.setVertexLayout(vertexLayout);
    mesh.vertices = {
        { 1.f, 0.f, 0xffff0000 },
        { .5f, 1.f, 0xff00ff00 },
        { 0.f, 0.f, 0xff0000ff },
    };

    stock::RenderState::configure();
    stock::RenderState::depthTest(false);

    stock::Log::setLevel(stock::Log::Level::verbose);

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(window)) {
        // Render here.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mesh.draw(shader);

        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for and process events.
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
