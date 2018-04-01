cmake_minimum_required(VERSION 3.2)

project(stock)

add_subdirectory(deps)

add_executable(stock
    src/gl/Error.cpp
    src/gl/Framebuffer.cpp
    src/gl/Mesh.cpp
    src/gl/Pixmap.cpp
    src/gl/RenderState.cpp
    src/gl/ShaderProgram.cpp
    src/gl/ShaderUniform.cpp
    src/gl/Texture.cpp
    src/gl/VertexLayout.cpp
    src/io/File.cpp
    src/io/Log.cpp
    src/view/Camera.cpp
    src/main.cpp
    src/ImGuiImpl.cpp
)

set_target_properties(stock PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(stock PRIVATE
    src
    deps/glm
    deps/stb
)

target_link_libraries(stock PRIVATE
    glad
    glfw
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    imgui
)
