#pragma once

#include "GLFW/glfw3.h"

namespace Marbas {

class ImguiInterface {
 public:
  ImguiInterface() = default;
  virtual ~ImguiInterface() = default;

 public:
  virtual void
  Resize() = 0;

  virtual void
  CreateImguiContext() = 0;

  virtual void
  ClearUp() = 0;

  virtual void
  SetUpImguiBackend(GLFWwindow* windows) = 0;

  virtual void
  NewFrame() = 0;

  virtual void
  RenderData(uint32_t width, uint32_t height) = 0;
};

}  // namespace Marbas
