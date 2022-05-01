#include "Layer/DrawLayer.hpp"
#include "Layer/RenderLayer.hpp"
#include "Core/Application.hpp"
#include "Layer/RenderLayer.hpp"
#include "Widget/RenderImage.hpp"
#include "Widget/SceneTree.hpp"
#include "Widget/MeshInfomationWidget.hpp"
#include "Core/Scene.hpp"
#include "Common.hpp"

#include <imgui.h>

namespace Marbas {

DrawLayer::DrawLayer(const Window* window, ResourceManager* resourceManager) :
    LayerBase(window),
    m_resourceManager(resourceManager)
{}

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
        "scene file (*.obj *.pmx *.glTF){.pmx,.obj,.glTF}",
    };

    auto renderLayer = m_window->GetRenderLayer();
    if(renderLayer == nullptr) {
        LOG(ERROR) << "can't find renderLayer";
        throw std::runtime_error("can't find renderLayer");
    }

    auto renderImage = std::make_unique<RenderImage>("renderImage");
    auto sceneTree = std::make_unique<SceneTreeWidget>();
    auto meshInfomationWidget = std::make_unique<MeshInfomationWidget>();

    sceneTree->AddSelectMeshWidget(renderImage.get());
    sceneTree->AddSelectMeshWidget(meshInfomationWidget.get());

    m_fileDialog= std::make_unique<FileDialog>(info);
    m_sceneFileDialog = std::make_unique<FileDialog>(sceneInfo);

    AddWidget(std::move(renderImage));
    AddWidget(std::move(sceneTree));
    AddWidget(std::move(meshInfomationWidget));
    // AddWidget(std::move(fileDialog));

    for(auto& widget : widgets){
        widget->SetResourceManager(m_resourceManager);
    }
}

void DrawLayer::OnUpdate() {
    auto* scene = m_window->GetRenderLayer()->GetScene();
    for(auto& widget : widgets) {
        ImGui::Begin(widget->GetWidgetName().c_str());
        widget->SetScene(scene);
        widget->Draw();
        ImGui::End();
    }
    DrawMenuBar();
}

void DrawLayer::DrawMenuBar() {

    m_fileDialog->SelectCallback([](const char* filePathName, const char* fileName){
    });

    m_sceneFileDialog->SelectCallback([&](const char* filePathName, const char* fileName) {
        auto scene = Scene::CreateSceneFromFile(filePathName, m_resourceManager);
        auto renderLayer = m_window->GetRenderLayer();
        renderLayer->SetSecne(std::move(scene));
    });

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                m_fileDialog->Open();
            }

            if (ImGui::MenuItem("打开", "Ctrl+S")) {
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
