## GLAD
add_subdirectory(glad)

## GLFW
# Configure GLFW to build only the library.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
add_subdirectory(glfw)
# Disable noisy deprecation warnings within CMake for macOS.
if(APPLE)
    target_compile_options(glfw PRIVATE "-Wno-deprecated-declarations")
endif()

## OpenGL
find_package(OpenGL REQUIRED)
