#ifndef MARBARS_CORE_APPLICATION_H
#define MARBARS_CORE_APPLICATION_H

#include <memory>
#include <mutex>

#include "Core/Window.h"
#include <glog/logging.h>

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

public:
    void Init() const;

    void CreateWindow(const WindowProp& winProp);

    void Run();

    void Destroy() {
        auto* origin = app.release();
        delete origin;
    }

private:
    Application() = default;

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

private:
    std::unique_ptr<Window> appWindow;
};

}

#endif
