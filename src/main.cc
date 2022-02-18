#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include "core/Application.h"
#include "core/Window.h"

#include <iostream>
#include <glog/logging.h>

int main(int argc, char* argv[]) {
    FLAGS_alsologtostderr = true;
    google::InitGoogleLogging(argv[0]);

    auto app = Marbas::Core::Application::GetInstace();

    Marbas::Core::WindowProp winProp {
        .name = "Marbas",
        .width = 800,
        .height = 600,
    };

    try {
        app->Init();
        app->CreateWindow(winProp);
        app->Run();
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    app->Destroy();
}
