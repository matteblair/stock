// ImGui GLFW binding with OpenGL3 + shaders
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

// Implemented features:
//  [X] User texture binding. Cast 'GLuint' OpenGL texture identifier as void*/ImTextureID. Read the FAQ about ImTextureID in imgui.cpp.
//  [X] Gamepad navigation mapping. Enable with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2018-03-06: OpenGL: Added const char* glsl_version parameter to ImGui_ImplGlfwGL3_Init() so user can override the GLSL version e.g. "#version 150".
//  2018-02-23: OpenGL: Create the VAO in the render function so the setup can more easily be used with multiple shared GL context.
//  2018-02-20: Inputs: Added support for mouse cursors (ImGui::GetMouseCursor() value and WM_SETCURSOR message handling).
//  2018-02-20: Inputs: Renamed GLFW callbacks exposed in .h to not include GL3 in their name.
//  2018-02-16: Misc: Obsoleted the io.RenderDrawListsFn callback and exposed ImGui_ImplGlfwGL3_RenderDrawData() in the .h file so you can call it yourself.
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.
//  2018-02-06: Inputs: Added mapping for ImGuiKey_Space.
//  2018-01-25: Inputs: Added gamepad support if ImGuiConfigFlags_NavEnableGamepad is set.
//  2018-01-25: Inputs: Honoring the io.WantMoveMouse by repositioning the mouse (when using navigation and ImGuiConfigFlags_NavMoveMouse is set).
//  2018-01-20: Inputs: Added Horizontal Mouse Wheel support.
//  2018-01-18: Inputs: Added mapping for ImGuiKey_Insert.
//  2018-01-07: OpenGL: Changed GLSL shader version from 330 to 150. (Also changed GL context from 3.3 to 3.2 in example's main.cpp)
//  2017-09-01: OpenGL: Save and restore current bound sampler. Save and restore current polygon mode.
//  2017-08-25: Inputs: MousePos set to -FLT_MAX,-FLT_MAX when mouse is unavailable/missing (instead of -1,-1).
//  2017-05-01: OpenGL: Fixed save and restore of current blend function state.
//  2016-10-15: Misc: Added a void* user_data parameter to Clipboard function handlers.
//  2016-09-05: OpenGL: Fixed save and restore of current scissor rectangle.
//  2016-04-30: OpenGL: Fixed save and restore of current GL_ACTIVE_TEXTURE.

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "ImGuiImpl.hpp"

#include "gl/GL.hpp"
#include "gl/Mesh.hpp"
#include "gl/ShaderProgram.hpp"
#include "gl/Texture.hpp"
#include "gl/VertexLayout.hpp"

// GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

namespace stock {

namespace ImGuiImpl {

// GLFW data
static GLFWwindow*  g_Window = NULL;
static double       g_Time = 0.0f;
static bool         g_MouseJustPressed[3] = { false, false, false };
static GLFWcursor*  g_MouseCursors[ImGuiMouseCursor_COUNT] = { 0 };

// OpenGL data
static Texture          g_FontTexture;
static ShaderProgram    g_ShaderProgram("", "");
static UniformLocation  g_TexUniformLocation("Texture"), g_ProjMtxUniformLocation("ProjMtx");
static VertexLayout     g_VertexLayout({
  VertexAttribute("Position", 2, GL_FLOAT, false),
  VertexAttribute("UV", 2, GL_FLOAT, false),
  VertexAttribute("Color", 4, GL_UNSIGNED_BYTE, true),
});
static Mesh<ImDrawVert> g_Mesh;

// OpenGL3 Render function.
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
// Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly, in order to be able to run within any OpenGL engine that doesn't do so.
void RenderDrawData(RenderState& rs, ImDrawData* draw_data)
{
  // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
  ImGuiIO& io = ImGui::GetIO();
  int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
  int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
  if (fb_width == 0 || fb_height == 0)
  {
    return;
  }
  draw_data->ScaleClipRects(io.DisplayFramebufferScale);

  // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
  rs.blending(true);
  rs.blendEquation(GL_FUNC_ADD);
  rs.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  rs.culling(false);
  rs.depthTest(false);
  rs.scissorTest(true);

  // Setup viewport, orthographic projection matrix
  glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
  const glm::mat4 ortho_projection =
  {
    { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
    { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
    { 0.0f,                  0.0f,                  -1.0f, 0.0f },
    {-1.0f,                  1.0f,                   0.0f, 1.0f },
  };
  g_ShaderProgram.setUniformMatrix4f(rs, g_ProjMtxUniformLocation, ortho_projection);
  g_ShaderProgram.setUniformi(rs, g_TexUniformLocation, 0);

  // Draw
  for (int n = 0; n < draw_data->CmdListsCount; n++)
  {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    const ImDrawIdx* idx_buffer_offset = 0;

    g_Mesh.reset();

    auto* vertices_start = cmd_list->VtxBuffer.Data;
    auto* vertices_end = vertices_start + cmd_list->VtxBuffer.Size;
    g_Mesh.vertices.assign(vertices_start, vertices_end);

    auto* indices_start = cmd_list->IdxBuffer.Data;
    auto* indices_end = indices_start + cmd_list->IdxBuffer.Size;
    g_Mesh.indices.assign(indices_start, indices_end);

    g_Mesh.upload(rs, GL_STREAM_DRAW);

    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
    {
      const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
      if (pcmd->UserCallback)
      {
        pcmd->UserCallback(cmd_list, pcmd);
      }
      else
      {
        rs.texture(GL_TEXTURE_2D, 0, (GLuint)(intptr_t)pcmd->TextureId);
        rs.scissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
        g_Mesh.draw(rs, g_ShaderProgram, pcmd->ElemCount, idx_buffer_offset);
      }
      idx_buffer_offset += pcmd->ElemCount;
    }
  }

  // Restore modified GL state. Might need to restore viewport??
  // glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

static const char* GetClipboardText(void* user_data)
{
  return glfwGetClipboardString((GLFWwindow*)user_data);
}

static void SetClipboardText(void* user_data, const char* text)
{
  glfwSetClipboardString((GLFWwindow*)user_data, text);
}

void MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/)
{
  if (action == GLFW_PRESS && button >= 0 && button < 3)
  {
    g_MouseJustPressed[button] = true;
  }
}

void ScrollCallback(GLFWwindow*, double xoffset, double yoffset)
{
  ImGuiIO& io = ImGui::GetIO();
  io.MouseWheelH += (float)xoffset;
  io.MouseWheel += (float)yoffset;
}

void KeyCallback(GLFWwindow*, int key, int, int action, int mods)
{
  ImGuiIO& io = ImGui::GetIO();
  if (action == GLFW_PRESS)
  {
    io.KeysDown[key] = true;
  }
  if (action == GLFW_RELEASE)
  {
    io.KeysDown[key] = false;
  }

  (void)mods; // Modifiers are not reliable across systems
  io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
  io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
  io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
  io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void CharCallback(GLFWwindow*, unsigned int c)
{
  ImGuiIO& io = ImGui::GetIO();
  if (c > 0 && c < 0x10000)
  {
    io.AddInputCharacter((unsigned short)c);
  }
}

bool CreateFontsTexture(RenderState& rs)
{
  // Build texture atlas
  ImGuiIO& io = ImGui::GetIO();
  unsigned char* pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

  // Upload texture to graphics system
  Texture::Options fontTexOptions;
  g_FontTexture.dispose(rs);
  g_FontTexture = Texture(Pixmap(width, height, pixels, Pixmap::PixelFormat::RGBA), fontTexOptions);
  g_FontTexture.prepare(rs, 0);

  // Store our identifier
  io.Fonts->TexID = (void *)(intptr_t)g_FontTexture.glHandle();

  return true;
}

bool CreateDeviceObjects(RenderState& rs)
{
  const GLchar* vertex_shader = R"SHADER_END(
    uniform mat4 ProjMtx;
    attribute vec2 Position;
    attribute vec2 UV;
    attribute vec4 Color;
    varying vec4 Frag_Color;
    varying vec2 Frag_UV;
    void main()
    {
      Frag_UV = UV;
      Frag_Color = Color;
      gl_Position = ProjMtx * vec4(Position.xy, 0.0, 1.0);
    }
    )SHADER_END";

  const GLchar* fragment_shader = R"SHADER_END(
    uniform sampler2D Texture;
    varying vec2 Frag_UV;
    varying vec4 Frag_Color;
    void main()
    {
      gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);
    }
    )SHADER_END";

  g_ShaderProgram.dispose(rs);
  g_ShaderProgram = ShaderProgram(fragment_shader, vertex_shader);
  g_ShaderProgram.build(rs);

  g_Mesh.setVertexLayout(g_VertexLayout);

  CreateFontsTexture(rs);

  return true;
}

void InvalidateDeviceObjects(RenderState& rs)
{
  g_Mesh.dispose(rs);
  g_ShaderProgram.dispose(rs);
  if (g_FontTexture.glHandle())
  {
    g_FontTexture.dispose(rs);
    ImGui::GetIO().Fonts->TexID = 0;
  }
}

static void InstallCallbacks(GLFWwindow* window)
{
  glfwSetMouseButtonCallback(window, MouseButtonCallback);
  glfwSetScrollCallback(window, ScrollCallback);
  glfwSetKeyCallback(window, KeyCallback);
  glfwSetCharCallback(window, CharCallback);
}

bool Init(GLFWwindow* window, bool install_callbacks)
{
  g_Window = window;

  // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
  ImGuiIO& io = ImGui::GetIO();
  io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
  io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
  io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
  io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
  io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
  io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
  io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
  io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
  io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
  io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
  io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
  io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
  io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
  io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
  io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
  io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
  io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

  io.SetClipboardTextFn = SetClipboardText;
  io.GetClipboardTextFn = GetClipboardText;
  io.ClipboardUserData = g_Window;
#ifdef _WIN32
  io.ImeWindowHandle = glfwGetWin32Window(g_Window);
#endif

  // Load cursors
  // FIXME: GLFW doesn't expose suitable cursors for ResizeAll, ResizeNESW, ResizeNWSE. We revert to arrow cursor for those.
  g_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  g_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
  g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  g_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
  g_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
  g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
  g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

  if (install_callbacks)
  {
    InstallCallbacks(window);
  }

  return true;
}

void Shutdown(RenderState& rs)
{
  // Destroy GLFW mouse cursors
  for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
  {
    glfwDestroyCursor(g_MouseCursors[cursor_n]);
  }
  memset(g_MouseCursors, 0, sizeof(g_MouseCursors));

  // Destroy OpenGL objects
  InvalidateDeviceObjects(rs);
}

void NewFrame(RenderState& rs)
{
  if (!g_FontTexture.glHandle())
  {
    CreateDeviceObjects(rs);
  }

  ImGuiIO& io = ImGui::GetIO();

  // Setup display size (every frame to accommodate for window resizing)
  int w, h;
  int display_w, display_h;
  glfwGetWindowSize(g_Window, &w, &h);
  glfwGetFramebufferSize(g_Window, &display_w, &display_h);
  io.DisplaySize = ImVec2((float)w, (float)h);
  io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

  // Setup time step
  double current_time =  glfwGetTime();
  io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
  g_Time = current_time;

  // Setup inputs
  // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
  if (glfwGetWindowAttrib(g_Window, GLFW_FOCUSED))
  {
    if (io.WantMoveMouse)
    {
      glfwSetCursorPos(g_Window, (double)io.MousePos.x, (double)io.MousePos.y);   // Set mouse position if requested by io.WantMoveMouse flag (used when io.NavMovesTrue is enabled by user and using directional navigation)
    }
    else
    {
      double mouse_x, mouse_y;
      glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
      io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
    }
  }
  else
  {
    io.MousePos = ImVec2(-FLT_MAX,-FLT_MAX);
  }

  for (int i = 0; i < 3; i++)
  {
    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[i] = g_MouseJustPressed[i] || glfwGetMouseButton(g_Window, i) != 0;
    g_MouseJustPressed[i] = false;
  }

  // Update OS/hardware mouse cursor if imgui isn't drawing a software cursor
  ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
  if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None)
  {
    glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  }
  else
  {
    glfwSetCursor(g_Window, g_MouseCursors[cursor] ? g_MouseCursors[cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
    glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  // Gamepad navigation mapping [BETA]
  memset(io.NavInputs, 0, sizeof(io.NavInputs));
  if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)
  {
    // Update gamepad inputs
    #define MAP_BUTTON(NAV_NO, BUTTON_NO)       { if (buttons_count > (BUTTON_NO) && buttons[(BUTTON_NO)] == GLFW_PRESS) io.NavInputs[(NAV_NO)] = 1.0f; }
    #define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) { float v = (axes_count > (AXIS_NO)) ? axes[(AXIS_NO)] : (V0); v = (v - (V0)) / ((V1) - (V0)); if (v > 1.0f) v = 1.0f; if (io.NavInputs[(NAV_NO)] < v) io.NavInputs[(NAV_NO)] = v; }
    int axes_count = 0, buttons_count = 0;
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
    MAP_BUTTON(ImGuiNavInput_Activate,   0);     // Cross / A
    MAP_BUTTON(ImGuiNavInput_Cancel,     1);     // Circle / B
    MAP_BUTTON(ImGuiNavInput_Menu,       2);     // Square / X
    MAP_BUTTON(ImGuiNavInput_Input,      3);     // Triangle / Y
    MAP_BUTTON(ImGuiNavInput_DpadLeft,   13);    // D-Pad Left
    MAP_BUTTON(ImGuiNavInput_DpadRight,  11);    // D-Pad Right
    MAP_BUTTON(ImGuiNavInput_DpadUp,     10);    // D-Pad Up
    MAP_BUTTON(ImGuiNavInput_DpadDown,   12);    // D-Pad Down
    MAP_BUTTON(ImGuiNavInput_FocusPrev,  4);     // L1 / LB
    MAP_BUTTON(ImGuiNavInput_FocusNext,  5);     // R1 / RB
    MAP_BUTTON(ImGuiNavInput_TweakSlow,  4);     // L1 / LB
    MAP_BUTTON(ImGuiNavInput_TweakFast,  5);     // R1 / RB
    MAP_ANALOG(ImGuiNavInput_LStickLeft, 0,  -0.3f,  -0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickRight,0,  +0.3f,  +0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickUp,   1,  +0.3f,  +0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickDown, 1,  -0.3f,  -0.9f);
    #undef MAP_BUTTON
    #undef MAP_ANALOG
  }

  // Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
  ImGui::NewFrame();
}

} // namespace ImguiImpl

} // namespace stock
