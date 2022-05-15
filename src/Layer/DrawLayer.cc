#include "Layer/DrawLayer.hpp"

#include <imgui.h>

#include "Common.hpp"
#include "Core/Application.hpp"
#include "Core/Scene/Scene.hpp"
#include "Layer/RenderLayer.hpp"
#include "Widget/MeshInfomationWidget.hpp"
#include "Widget/RenderImage.hpp"
#include "Widget/SceneTree.hpp"

namespace Marbas {

DrawLayer::DrawLayer(const Window* window, ResourceManager* resourceManager)
    : LayerBase(window), m_resourceManager(resourceManager) {}

DrawLayer::~DrawLayer() = default;

void DrawLayer::OnAttach() {
  auto renderLayer = m_window->GetRenderLayer();
  if (renderLayer == nullptr) {
    LOG(ERROR) << "can't find renderLayer";
    throw std::runtime_error("can't find renderLayer");
  }

  auto renderImage = std::make_unique<RenderImage>("renderImage");
  auto sceneTree = std::make_unique<SceneTreeWidget>();
  auto meshInfomationWidget = std::make_unique<MeshInfomationWidget>();

  sceneTree->AddSelectMeshWidget(renderImage.get());
  sceneTree->AddSelectMeshWidget(meshInfomationWidget.get());

  AddWidget(std::move(renderImage));
  AddWidget(std::move(sceneTree));
  AddWidget(std::move(meshInfomationWidget));

  for (auto& widget : widgets) {
    widget->SetResourceManager(m_resourceManager);
  }
}

void DrawLayer::OnUpdate() {
  auto* scene = m_window->GetRenderLayer()->GetScene();
  for (auto& widget : widgets) {
    ImGui::Begin(widget->GetWidgetName().c_str());
    widget->SetScene(scene);
    widget->Draw();
    ImGui::End();
  }
  DrawMenuBar();
}

void DrawLayer::DrawMenuBar() {
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

}  // namespace Marbas
