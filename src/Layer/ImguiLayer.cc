#include "Common.hpp"
#include "Layer/ImguiLayer.hpp"
#include "Core/Application.hpp"

#include <glog/logging.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>
#include <IconsFontAwesome6.h>

namespace Marbas {

ImguiLayer::~ImguiLayer() {
    LOG(INFO) << "release ImguiLayer";
}

void ImguiLayer::OnAttach() {
    LOG(INFO) << "begin to initialize imgui layer";

    m_rhiFactory = Application::GetRendererFactory();
    m_viewport = m_rhiFactory->CreateViewport();

    const char* glsl_version = "#version 150";

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

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

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF( "assert/font/fa-solid-900.ttf", 16.0f, &icons_config, icons_ranges);


    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg
    // so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window->GetGlfwWinow(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    LOG(INFO) << "imgui layer initialized successful";
}

void ImguiLayer::OnDetach() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImguiLayer::OnBegin() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuizmo::BeginFrame();
}

void ImguiLayer::OnEnd() {

    ImGui::Render();

    auto [display_w, display_h] = Application::GetApplicationsWindow()->GetWindowsSize();

    m_viewport->SetViewport(0, 0, display_w, display_h);
    m_viewport->UseViewport();

    m_rhiFactory->ClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                             clear_color.z * clear_color.w, clear_color.w);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

}  // namespace Marbas
