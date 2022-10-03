#include <imgui.h>
#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include <GLFW/glfw3.h>
#include <glog/logging.h>

#include "RHI/Interface/RHIFactory.hpp"

int
main(int argc, char* argv[]) {
  FLAGS_alsologtostderr = true;
  google::InitGoogleLogging(argv[0]);

  // init glfw glfwWindow  and rhiFactory
  int width = 800;
  int height = 600;
  glfwInit();
  auto* rhiFactory = Marbas::RHIFactory::GetInstance(Marbas::RendererType::VULKAN);

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  auto* glfwWindow = glfwCreateWindow(width, height, "Test Window", nullptr, nullptr);
  rhiFactory->SetGLFWwindow(glfwWindow);
  rhiFactory->Init(Marbas::RHICreateInfo{
      .m_openglRHICreateInfo = Marbas::OpenGLRHICreateInfo{.useSPIRV = false},
  });

  auto imguiInterface = rhiFactory->CreateImguiInterface();

  imguiInterface->CreateImguiContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  imguiInterface->SetUpImguiBackend(glfwWindow);

  while (!glfwWindowShouldClose(glfwWindow)) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui
    // wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main
    // application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main
    // application, or clear/overwrite your copy of the keyboard data. Generally you may always pass
    // all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();
    imguiInterface->Resize();

    imguiInterface->NewFrame();
    ImGui::ShowDemoWindow();

    glfwGetFramebufferSize(glfwWindow, &width, &height);
    imguiInterface->RenderData(width, height);
  }

  imguiInterface->ClearUp();

  return 0;
}
