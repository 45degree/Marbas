#include <imgui.h>
#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include <GLFW/glfw3.h>
#include <glog/logging.h>

#include <vulkan/vulkan.hpp>

#include "RHI/Interface/RHIFactory.hpp"
#include "RHI/Vulkan/VulkanRHIFactory.hpp"
#include "RHI/Vulkan/VulkanSwapChain.hpp"

static void
DrawImGUI(Marbas::ImguiInterface* imguiInterface) {
  imguiInterface->NewFrame();

  ImGui::ShowDemoWindow();
  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named
  // window.
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!" and append into it.

    ImGui::Text(
        "This is some useful text.");  // Display some text (you can use a format strings too)

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f

    if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return true
                                  // when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::End();
  }
  {
    // 3. Show another simple window.
    ImGui::Begin("Another Window");  // Pass a pointer to our bool variable (the window will have
                                     // a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    ImGui::End();
  }

  ImGui::Render();
}

int
main(int argc, char* argv[]) {
  FLAGS_alsologtostderr = true;
  google::InitGoogleLogging(argv[0]);

  // init glfw glfwWindow  and rhiFactory
  int width = 800;
  int height = 600;
  glfwInit();
  auto* rhiFactory = Marbas::RHIFactory::GetInstance(Marbas::RendererType::VULKAN);

  auto* glfwWindow = glfwCreateWindow(width, height, "Test Window", nullptr, nullptr);
  rhiFactory->SetGLFWwindow(glfwWindow);
  rhiFactory->Init(Marbas::RHICreateInfo{
      .m_openglRHICreateInfo = Marbas::OpenGLRHICreateInfo{.useSPIRV = false},
  });

  auto imguiInterface = rhiFactory->CreateImguiInterface();
  auto swapChain = rhiFactory->GetSwapChain();

  imguiInterface->CreateImguiContext();
  ImGuiIO& io = ImGui::GetIO();
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

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  imguiInterface->SetUpImguiBackend(glfwWindow);

  std::vector<Marbas::Semaphore> avaliableImageSemaphore;
  std::vector<Marbas::Semaphore> renderFinishSemaphore;

  for (int i = 0; i < swapChain->GetImageCount(); i++) {
    avaliableImageSemaphore.push_back(rhiFactory->CreateSemaphore());
    renderFinishSemaphore.push_back(rhiFactory->CreateSemaphore());
  }

  int currentFrame = 0;
  bool needResize = false;
  while (!glfwWindowShouldClose(glfwWindow)) {
    if (needResize) {
      glfwGetFramebufferSize(glfwWindow, &width, &height);

      if (width > 0 && height > 0) {
        swapChain->Resize(width, height);
        imguiInterface->Resize(width, height);
        currentFrame = 0;
        needResize = false;
        continue;
      }
    }

    int w, h;
    glfwGetWindowSize(glfwWindow, &w, &h);
    const bool is_minimized = (w <= 0.0f || h <= 0.0f);
    if (!is_minimized) {
      auto imageIndex = swapChain->AcquireNextImage(avaliableImageSemaphore[currentFrame]);
      if (imageIndex == -1) {
        needResize = true;
        continue;
      }

      DrawImGUI(imguiInterface.get());

      imguiInterface->RenderData(avaliableImageSemaphore[currentFrame], renderFinishSemaphore[0],
                                 imageIndex);

      if (swapChain->Present({renderFinishSemaphore[0]}, imageIndex) == -1) {
        needResize = true;
        continue;
      }
    }
    currentFrame = (currentFrame + 1) % swapChain->GetImageCount();

    glfwPollEvents();
  }

  imguiInterface->ClearUp();

  for (const auto& semaphore : avaliableImageSemaphore) {
    rhiFactory->DestroySemaphore(semaphore);
  }

  for (const auto& semaphore : renderFinishSemaphore) {
    rhiFactory->DestroySemaphore(semaphore);
  }

  return 0;
}
