#pragma once

#include "RHI/Interface/ImguiInterface.hpp"

namespace Marbas {

class OpenGLImguiInterface final : public ImguiInterface {
 public:
  OpenGLImguiInterface() = default;
  ~OpenGLImguiInterface() override = default;

 public:
  void
  Resize() override {}

  void
  CreateImguiContext() override;

  void
  ClearUp() override;

  void
  SetUpImguiBackend(GLFWwindow* windows) override;

  void
  NewFrame() override;

  void
  RenderData(uint32_t width, uint32_t height) override;
};

}  // namespace Marbas
