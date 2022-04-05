#include "Layer/DrawLayer.h"
#include "Core/Application.h"
#include "Core/Model.h"
#include "Layer/RenderLayer.h"
#include "Widget/RenderImage.h"
#include "Widget/SceneTree.h"
#include "Core/Sence.h"
#include "Common.h"

#include <imgui.h>

namespace Marbas {

DrawLayer::DrawLayer(): Layer("DrawLayer") {}

DrawLayer::~DrawLayer() = default;

void DrawLayer::OnAttach() {

    FileDialogCrateInfo info {
        "TextureOpenDialog",
        "Open a texture",
        "model file (*.off *.obj *.ply *.pmx){.off,.obj,.ply,.pmx}",
    };

    FileDialogCrateInfo sceneInfo {
        "Scene",
        "Open A Scene",
        "scene file (*.3DS *.glTF){.3DS,.glTF}",
    };

    auto renderImage = std::make_unique<Image>("renderImage");
    auto sceneTree = std::make_unique<SceneTreeWidget>();

    m_fileDialog= std::make_unique<FileDialog>(info);
    m_sceneFileDialog = std::make_unique<FileDialog>(sceneInfo);
    // m_fileDialog->SetShow(false);

    AddWidget(std::move(renderImage));
    AddWidget(std::move(sceneTree));
    // AddWidget(std::move(fileDialog));
}

void DrawLayer::OnUpdate() {
    for(auto& widget : widgets) {
        ImGui::Begin(widget->GetWidgetName().c_str());
        widget->Draw();
        ImGui::End();
    }
    DrawMenuBar();
}

void DrawLayer::DrawMenuBar() {

    m_fileDialog->SelectCallback([](const char* filePathName, const char* fileName){
        // auto model = std::make_unique<Model>();
        // model->CreateFromFile(filePathName);
        //
        // auto layer = Application::GetApplicationsWindow()->GetLayer("RenderLayer");
        // auto renderLayer = dynamic_cast<RenderLayer*>(layer);
        // renderLayer->AddModle(std::move(model));
    });

    m_sceneFileDialog->SelectCallback([](const char* filePathName, const char* fileName) {
        Scene::CreateSceneFromFile(filePathName);
    });

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                m_fileDialog->Open();
            }

            if (ImGui::MenuItem("打开", "Ctrl+S"))   {
        m_sceneFileDialog->Open();
      }

      if (ImGui::MenuItem("Close", "Ctrl+W")) {
      }

      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
  m_fileDialog->Draw();
  m_sceneFileDialog->Draw();
}

} // namespace Marbas
