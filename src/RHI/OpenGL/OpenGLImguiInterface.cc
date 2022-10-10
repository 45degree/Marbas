#include "RHI/OpenGL/OpenGLImguiInterface.hpp"

#include <glog/logging.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

void
OpenGLImguiInterface::CreateImguiContext() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  LOG(INFO) << "create opengl imgui context";
}

void
OpenGLImguiInterface::ClearUp() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void
OpenGLImguiInterface::SetUpImguiBackend(GLFWwindow* window) {
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

void
OpenGLImguiInterface::NewFrame() {
  GLint currentBinding = 0;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentBinding);
  if (currentBinding != 0) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void
OpenGLImguiInterface::RenderData(const Semaphore&, const Semaphore&, uint32_t) {
  glViewport(0, 0, m_width, m_height);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace Marbas
