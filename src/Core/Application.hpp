#pragma once

#include <glog/logging.h>

#include <memory>
#include <mutex>

#include "Core/Window.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

struct ApplicationData {
  RendererType rendererType = RendererType::OPENGL;
};

class Application {
 private:
  static std::unique_ptr<Application> app;

 public:
  static Application*
  GetInstace(std::unique_ptr<ApplicationData>&& appData = nullptr) {
    static std::once_flag flag;
    std::call_once(flag, [&]() { app.reset(new Application(std::move(appData))); });

    return app.get();
  }

  static std::shared_ptr<Window>
  GetApplicationsWindow() {
    return app->GetWindow();
  }

  static RHIFactory*
  GetRendererFactory() {
    if (!app->m_isInitialized) {
      String errorMsg = "can't get renderer factory before the application initialized";
      LOG(ERROR) << errorMsg;
      throw std::runtime_error(errorMsg.c_str());
    }
    auto rendererType = app->m_applicationData->rendererType;
    return RHIFactory::GetInstance(rendererType);
  }

 public:
  void
  Init();

  void
  CreateAppWindow(const WindowProp& winProp);

  void
  Run();

  void
  Destroy() {
    auto* origin = app.release();
    delete origin;
  }

  [[nodiscard]] std::shared_ptr<Window>
  GetWindow() const {
    return m_appWindow;
  }

 private:
  explicit Application(std::unique_ptr<ApplicationData>&& appData)
      : m_applicationData(std::move(appData)) {}

 public:
  Application(const Application&) = delete;

  Application&
  operator=(const Application&) = delete;

 private:
  std::shared_ptr<Window> m_appWindow;
  std::unique_ptr<ApplicationData> m_applicationData;

  bool m_isInitialized = false;
};

}  // namespace Marbas
