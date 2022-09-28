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
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  auto* glfwWindow = glfwCreateWindow(width, height, "Test Window", nullptr, nullptr);
  rhiFactory->SetGLFWwindow(glfwWindow);
  rhiFactory->Init(Marbas::RHICreateInfo{
      .m_openglRHICreateInfo = Marbas::OpenGLRHICreateInfo{.useSPIRV = false},
  });

  return 0;
}
