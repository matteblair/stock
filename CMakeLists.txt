cmake_minimum_required(VERSION 3.2)

project(stock)

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
    )

set_target_properties(stock PROPERTIES CXX_STANDARD 11)

target_include_directories(stock PRIVATE src)

## GLAD ##
add_subdirectory(deps/glad)
target_link_libraries(stock PRIVATE glad)

## GLFW ##
# Configure GLFW to build only the library.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
add_subdirectory(deps/glfw)
# Disable noisy deprecation warnings within CMake for macOS.
if(APPLE)
    target_compile_options(glfw PRIVATE "-Wno-deprecated-declarations")
endif()
target_link_libraries(stock PRIVATE glfw ${GLFW_LIBRARIES})

## OpenGL ##
find_package(OpenGL REQUIRED)
target_link_libraries(stock PRIVATE ${OPENGL_LIBRARIES})

## GLM ##
target_include_directories(stock PRIVATE deps/glm)

## STB ##
target_include_directories(stock PRIVATE deps/stb)
