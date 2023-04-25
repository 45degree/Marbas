#pragma once

#include "Common/Common.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

struct StartUpSetting {};

class StartUpApplication final {
 public:
  StartUpApplication();
  ~StartUpApplication() = default;

  int
  Initialize();

  void
  Run();

  void
  Quit();

  const std::optional<Path>&
  GetProjectDir() const {
    return m_projectDir;
  }

 private:
  GLFWwindow* m_glfwWindow;
  std::optional<Path> m_projectDir = std::nullopt;
};

}  // namespace Marbas
