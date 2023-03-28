#include <glog/logging.h>

#include <iostream>

#include "Application.hpp"
#include "StartUp/StartUpApplication.hpp"

int
main(int argc, char* argv[]) {
  // TODO: change the language, but should set the encoding as utf8
  std::locale::global(std::locale("zh_CN.utf8"));

  FLAGS_alsologtostderr = true;
  google::InitGoogleLogging(argv[0]);

  Marbas::StartUpApplication startUpApp;
  startUpApp.Initialize();
  try {
    startUpApp.Run();
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  startUpApp.Quit();

  const auto& projectDir = startUpApp.GetProjectDir();
  if (projectDir == std::nullopt) return 0;

  Marbas::ApplicationData appData;
  appData.rendererType = Marbas::RendererType::VULKAN;
  appData.projectDir = *projectDir;

  auto app = std::make_unique<Marbas::Application>(appData);
  app->Initialize();

  // try {
  app->Run();
  // } catch (const std::exception& e) {
  //   std::cout << e.what() << std::endl;
  // }

  app->Quit();

  return 0;
}
