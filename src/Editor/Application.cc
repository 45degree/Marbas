#include "Application.hpp"

#include <IconsFontAwesome6.h>
#include <glog/logging.h>

// clang-format off
#include <imgui/imgui.h>
#include <ImGuizmo.h>
#include <chrono>
// clang-format on

#include "AssetManager/AssetRegistry.hpp"
#include "Common/Common.hpp"
#include "Core/Scene/GPUDataPipeline/GPUDataManager.hpp"
#include "Core/Scene/SceneManager.hpp"
#include "Core/Scene/System/RenderSystem.hpp"
#include "Editor/Widget/ContentBrowser.hpp"
#include "GLFW/glfw3.h"
#include "Widget/CommonName.hpp"
#include "Widget/InformationWidget.hpp"
#include "Widget/RenderImage.hpp"
#include "Widget/SceneTree.hpp"

namespace Marbas {

Application::Application(const ApplicationData& appData) {
  glfwSetErrorCallback([](int error, const char* descriptor) {
    LOG(ERROR) << FORMAT("glfw error {}: {}", error, descriptor);
    return;
  });

  if (!glfwInit()) {
    throw std::runtime_error("failed to initialized glfw");
  }

  glfwWindowHint(GLFW_MAXIMIZED, 1);
  m_rhiFactory = RHIFactory::CreateInstance(appData.rendererType);
  m_glfwWindow = glfwCreateWindow(appData.width, appData.height, appData.windowName.c_str(), nullptr, nullptr);
  if (m_glfwWindow == nullptr) {
    throw std::runtime_error("failed to create window");
  }

  m_rhiFactory->Init(m_glfwWindow, appData.width, appData.height);

  m_swapChain = m_rhiFactory->GetSwapchain();
  for (int i = 0; i < m_swapChain->imageViews.size(); i++) {
    m_aviableSemaphores.push_back(m_rhiFactory->CreateGPUSemaphore());
    m_finishSemaphores.push_back(m_rhiFactory->CreateGPUSemaphore());
  }
  m_renderFinishSemaphore = m_rhiFactory->CreateGPUSemaphore();
  m_imguiFinishSemaphore = m_rhiFactory->CreateGPUSemaphore();

  m_imguiContext = m_rhiFactory->GetImguiContext();
  m_imguiContext->SetRenderResultImage(m_swapChain->imageViews);

  m_frameFence = m_rhiFactory->CreateFence();

  auto registry = AssetRegistry::GetInstance();
  registry->SetProjectDir(appData.projectDir);
}

void
Application::Initialize() {
  /**
   * set up imgui
   */

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

  static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  ImFontConfig icons_config;
  icons_config.MergeMode = true;
  icons_config.PixelSnapH = true;
  io.Fonts->AddFontFromFileTTF("assert/font/fa-solid-900.ttf", 12.0f, &icons_config, icons_ranges);

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform
  // Windows
  // io.ConfigViewportsNoAutoMerge = true;
  // io.ConfigViewportsNoTaskBarIcon = true;

  // Setup Dear ImGui style
  ImGui::StyleColorsClassic();

  // When viewports are enabled we tweak WindowRounding/WindowBg
  // so platform windows can look identical to regular ones.
  ImGuiStyle& style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  // Setup Platform/Renderer backends
  m_imguiContext->SetUpImguiBackend(m_glfwWindow);

  // create resource manager
  // m_scene = std::make_unique<Scene>();
  auto sceneManager = SceneManager::GetInstance();
  auto scene = sceneManager->CreateEmptyScene();
  sceneManager->SetActiveScene(scene);

  // create render engine
  GPUDataManager::SetUp(m_rhiFactory.get());
  RenderSystem::Initialize(m_rhiFactory.get());
  RenderSystem::CreateRenderGraph(m_rhiFactory.get());
}

void
Application::Run() {
  int currentFrame = 0;
  bool needResize = false;

  SceneTreeWidget sceneTree(m_rhiFactory.get());
  RenderImage renderImage(m_rhiFactory.get());
  InformationWidget infoWidget(m_rhiFactory.get());
  ContentBrowser ContentBrowser(m_rhiFactory.get());
  sceneTree.m_selectEntity.Connect<&RenderImage::SetSelectedEntity>(renderImage);
  sceneTree.m_selectEntity.Connect<&InformationWidget::SelectEntity>(infoWidget);

  auto outputImageView = RenderSystem::GetOutputView();
  renderImage.SetRenderImage(outputImageView);

  RenderSystem::RegistryListenerForResultImageChange<&RenderImage::SetRenderImage>(renderImage);

  while (!glfwWindowShouldClose(m_glfwWindow)) {
    glfwPollEvents();
    m_rhiFactory->ResetFence(m_frameFence);

    if (needResize) {
      int width, height;
      glfwGetFramebufferSize(m_glfwWindow, &width, &height);

      if (width > 0 && height > 0) {
        m_rhiFactory->WaitIdle();
        m_swapChain = m_rhiFactory->RecreateSwapchain(m_swapChain, width, height);
        m_imguiContext->SetRenderResultImage(m_swapChain->imageViews);
        m_imguiContext->Resize(width, height);
        currentFrame = 0;
        needResize = false;
        continue;
      }
    }

    int w, h;
    glfwGetWindowSize(m_glfwWindow, &w, &h);
    const bool is_minimized = (w <= 0 || h <= 0);
    if (is_minimized) continue;
    auto imageIndex = m_rhiFactory->AcquireNextImage(m_swapChain, m_aviableSemaphores[currentFrame]);
    if (imageIndex == -1) {
      needResize = true;
      continue;
    }

    if (renderImage.needBakeScene) {
      renderImage.needBakeScene = false;
    }

    auto sceneManager = SceneManager::GetInstance();
    auto scene = sceneManager->GetActiveScene();

    // render scene
    {
      SceneSystem::UpdateEveryFrame(scene, m_rhiFactory.get());

      RenderSystem::Update(RenderInfo{
          .scene = scene,
          .imageIndex = currentFrame,
          .waitSemaphore = m_aviableSemaphores[currentFrame],
          .signalSemaphore = m_renderFinishSemaphore,
          .fence = m_frameFence,
      });
    }

    // draw imgui
    {
      BeginImgui();

      // TODO: Draw ImGui
      ImGui::Begin("tree");
      sceneTree.Draw();
      ImGui::End();

      if (ImGui::BeginDragDropTarget()) {
        if (auto* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAGDROG); payload != nullptr) {
          const auto& path = *reinterpret_cast<AssetPath*>(payload->Data);
          // TODO: check if the path is a scene path
          auto sceneManager = SceneManager::GetInstance();
          sceneManager->LoadScene(path);

          auto scene = sceneManager->GetScene(path);
          sceneManager->SetActiveScene(scene);
          sceneTree.m_selectEntity.Publish(entt::null);
        }
        ImGui::EndDragDropTarget();
      }

      ImGui::Begin("image");
      renderImage.Draw();
      ImGui::End();
      ImGui::Begin("Info");
      infoWidget.Draw();
      ImGui::End();
      ImGui::Begin("ContentBrowser");
      ContentBrowser.Draw();
      ImGui::End();

      // use ctrl + s to store the scene
      if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
        if (ImGui::IsKeyDown(ImGuiKey_S)) {
          // if (sceneManager->IsUnNamedScene(scene)) {
          sceneManager->SaveScene("res://scene.scene", scene);
          // }
        }
      }

      EndImgui(imageIndex, m_renderFinishSemaphore, m_finishSemaphores[currentFrame]);
    }

    // present result
    {
      auto presentResult = m_rhiFactory->Present(m_swapChain, {&m_finishSemaphores[currentFrame], 1}, imageIndex);
      m_rhiFactory->WaitForFence(m_frameFence);
      if (-1 == presentResult) {
        needResize = true;
        continue;
      }
      currentFrame = (currentFrame + 1) % m_swapChain->imageViews.size();
    }
  }
}

void
Application::Quit() {
  m_rhiFactory->WaitIdle();

  // Clear AssetManager
  GPUDataManager::TearDown();

  // clear context
  m_imguiContext->ClearUp();

  m_rhiFactory->DestroyFence(m_frameFence);
  RenderSystem::Destroy(m_rhiFactory.get());
}

void
Application::BeginImgui() {
  // Start the Dear ImGui frame
  m_imguiContext->NewFrame();
  ImGuizmo::BeginFrame();
  BeginImGuiDocking();
}

void
Application::BeginImGuiDocking() {
  /**
   * set dockingspace
   */
  static bool opt_fullscreen = true;
  static bool opt_padding = false;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
  // because it would be confusing to have two docking targets within each others.
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  if (opt_fullscreen) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |=
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  } else {
    dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
  }

  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
  // and handle the pass-thru hole, so we ask Begin() to not render a background.
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
    window_flags |= ImGuiWindowFlags_NoBackground;
  }

  // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
  // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
  // all active windows docked into it will lose their parent and become undocked.
  // We cannot preserve the docking relationship between an active window and an inactive docking,
  // otherwise any change of dockspace/settings would lead to windows being stuck in limbo and
  // never being visible.
  static bool p_open = true;
  if (!opt_padding) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  }
  ImGui::Begin("DockSpace Demo", &p_open, window_flags);
  if (!opt_padding) {
    ImGui::PopStyleVar();
  }

  if (opt_fullscreen) {
    ImGui::PopStyleVar(2);
  }

  // Submit the DockSpace
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }
}

void
Application::EndImgui(int imageIndex, Semaphore* waitSemaphores, Semaphore* signalSemaphores) {
  EndImGuiDocking();
  ImGui::Render();

  ImguiRenderDataInfo renderInfo;
  renderInfo.waitSemaphore = waitSemaphores;
  renderInfo.signalSemaphore = signalSemaphores;
  m_imguiContext->RenderData(imageIndex, renderInfo);

  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

void
Application::EndImGuiDocking() {
  ImGui::End();
}

}  // namespace Marbas
