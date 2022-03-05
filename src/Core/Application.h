#ifndef MARBARS_CORE_APPLICATION_H
#define MARBARS_CORE_APPLICATION_H

#include <memory>
#include <mutex>

#include "Core/Window.h"

namespace Marbas {

class Application {
private:
    static std::unique_ptr<Application> app;

public:
    static Application* GetInstace() {
        static std::once_flag flag;
        std::call_once(flag, [&]() {
            app.reset(new Application());
        });

        return app.get();
    }

    static const Window* GetApplicationsWindow() {
        return app->GetWindow();
    }

public:
    void Init() const;

    void CreateSingleWindow(const WindowProp& winProp);

    void Run();

    void Destroy() {
        auto* origin = app.release();
        delete origin;
    }

    [[nodiscard]] const Window* GetWindow() const {
        return appWindow.get();
    }

private:
    Application() = default;

public:
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

private:
    std::unique_ptr<Window> appWindow;
};

}  // namespace Marbas

#endif
