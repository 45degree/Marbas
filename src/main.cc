#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include "Core/Application.h"
#include "Core/Window.h"

#include <iostream>

int main(int argc, char* argv[]) {
    FLAGS_alsologtostderr = true;
    google::InitGoogleLogging(argv[0]);

    // read config file

    auto appData = std::make_unique<Marbas::ApplicationData>();
    appData->rendererType = Marbas::RendererType::OPENGL;

    auto app = Marbas::Application::GetInstace(std::move(appData));

    Marbas::WindowProp winProp;
    winProp.name = "Marbas";
    winProp.width = 800;
    winProp.height = 600;

    try {
        app->Init();
        app->CreateSingleWindow(winProp);
        app->Run();
    }
    catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    app->Destroy();
}
