#define GOOGLE_GLOG_DLL_DECL
#define GLOG_NO_ABBREVIATED_SEVERITIES

#include <iostream>

#include "App/Editor/Layer/DrawLayer.hpp"
#include "Config.hpp"
#include "Core/Application.hpp"
#include "Core/Window.hpp"

int
main(int argc, char* argv[]) {
  FLAGS_alsologtostderr = true;
  google::InitGoogleLogging(argv[0]);

  // read config file
  // Marbas::Config config("assert/");
  // config.Load();

  auto appData = std::make_unique<Marbas::ApplicationData>();
  appData->rendererType = Marbas::RendererType::OPENGL;

  auto app = Marbas::Application::GetInstace(std::move(appData));

  Marbas::WindowProp winProp;
  winProp.name = "Marbas";
  winProp.width = 800;
  winProp.height = 600;

  app->Init();
  app->CreateAppWindow(winProp);

  // add draw layer
  auto window = app->GetWindow();
  auto drawLayer = std::make_shared<Marbas::DrawLayer>(window, window->GetImGuiLayer());
  window->SetCustomLayer(drawLayer);

  window->InitializeWindow();

  try {
    app->Run();
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  app->Destroy();
}
