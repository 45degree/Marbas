// clang-format off
#include <GLFW/glfw3.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/GL.h>
#else
#include <GL/gl.h>
#endif

// clang-format on

#include "StartUpApplication.hpp"

#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include "StartUpPlane.hpp"

namespace Marbas {

static void
SetStyle() {
  auto& style = ImGui::GetStyle();
  style.Colors[ImGuiCol_ButtonActive] = ImColor(41, 40, 41, 255);
  style.Colors[ImGuiCol_Button] = ImColor(31, 30, 31, 255);
}

// Our state
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

StartUpApplication::StartUpApplication() {}

int
StartUpApplication::Initialize() {
  if (!glfwInit()) return -1;

#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char* glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
  glfwWindowHint(GLFW_RESIZABLE, false);

  // Create window with graphics context
  m_glfwWindow = glfwCreateWindow(800, 500, "Marbas StartUp Menu", NULL, NULL);
  if (m_glfwWindow == NULL) return -1;

  glfwMakeContextCurrent(m_glfwWindow);
  glfwSwapInterval(1);  // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = "startup.ini";
  (void)io;

  ImFontConfig cfg;
  cfg.OversampleH = 1;
  static ImFontGlyphRangesBuilder range;
  range.Clear();
  static ImVector<ImWchar> gr;
  gr.clear();
  range.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());
  range.BuildRanges(&gr);

  io.Fonts->AddFontFromFileTTF("assert/font/NotoSansSC-Light.otf", 16, &cfg, gr.Data);

  static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  ImFontConfig icons_config;
  icons_config.MergeMode = true;
  icons_config.PixelSnapH = true;
  io.Fonts->AddFontFromFileTTF("assert/font/fa-solid-900.ttf", 16.0f, &icons_config, icons_ranges);

  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  SetStyle();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  return 1;
}

void
StartUpApplication::Run() {
  while (!glfwWindowShouldClose(m_glfwWindow) && !m_projectDir) {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::PopStyleVar(2);

    ImGui::Begin("startup", nullptr, window_flags);
    StartUpPlane plane;
    plane.Render();
    m_projectDir = plane.m_projectPath;
    ImGui::End();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(m_glfwWindow, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                 clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_glfwWindow);
  }
}

void
StartUpApplication::Quit() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(m_glfwWindow);
  glfwTerminate();
}

}  // namespace Marbas
