#include "Core/Layer/ImguiLayer.hpp"

#include <IconsFontAwesome6.h>
#include <ImGuizmo.h>
#include <glog/logging.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Common/Common.hpp"
#include "Core/Window.hpp"

namespace Marbas {

ImguiLayer::~ImguiLayer() { LOG(INFO) << "release ImguiLayer"; }

void
ImguiLayer::OnAttach() {
  LOG(INFO) << "begin to initialize imgui layer";

  m_imguiInterface = m_rhiFactory->CreateImguiInterface();

  // Setup Dear ImGui context
  m_imguiInterface->CreateImguiContext();

  ImGuiIO& io = ImGui::GetIO();
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

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform
  // Windows
  // io.ConfigViewportsNoAutoMerge = true;
  // io.ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // When viewports are enabled we tweak WindowRounding/WindowBg
  // so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  if (m_window.expired()) {
    throw std::runtime_error("m_window is an empty pointer");
  }
  m_imguiInterface->SetUpImguiBackend(m_window.lock()->GetGlfwWinow());

  LOG(INFO) << "imgui layer initialized successful";
}

void
ImguiLayer::OnDetach() {
  m_imguiInterface->ClearUp();
}

void
ImguiLayer::OnBegin() {
  // Start the Dear ImGui frame
  m_imguiInterface->NewFrame();
  ImGuizmo::BeginFrame();
}

void
ImguiLayer::OnEnd() {
  ImGui::Render();

  if (m_window.expired()) {
    throw std::runtime_error("m_window is an empty pointer");
  }
  auto [display_w, display_h] = m_window.lock()->GetWindowsSize();

  m_imguiInterface->RenderData(display_w, display_h);

  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

ImGuiContext*
ImguiLayer::GetCurrentContext() {
  return ImGui::GetCurrentContext();
}

}  // namespace Marbas
