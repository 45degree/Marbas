#include "App/Editor/Layer/DrawLayer.hpp"

#include <nfd.h>

#include "App/Editor/Widget/InfomationWidget.hpp"
#include "App/Editor/Widget/RenderImage.hpp"
#include "App/Editor/Widget/SceneTree.hpp"
#include "App/Editor/Widget/Widget.hpp"
#include "Core/Window.hpp"

namespace Marbas {

void
DrawLayer::OnAttach() {
  ImGui::SetCurrentContext(m_imguiLayer->GetCurrentContext());

  auto renderImage = std::make_unique<RenderImage>(m_rhiFactory, "renderImage");
  auto sceneTree = std::make_unique<SceneTreeWidget>(m_rhiFactory);
  auto meshInfomationWidget = std::make_unique<InformationWidget>(m_rhiFactory);

  sceneTree->Connect<&RenderImage::SetSelectedModel>(*renderImage);
  sceneTree->Connect<&InformationWidget::SelectEntity>(*meshInfomationWidget);

  AddWidget(std::move(renderImage));
  AddWidget(std::move(sceneTree));
  AddWidget(std::move(meshInfomationWidget));

  for (auto& widget : widgets) {
    widget->SetResourceManager(m_resourceManager);
  }
}

void
DrawLayer::OnUpdate(const GlobalLayerInfo&) {
  if (m_window.expired()) {
    throw std::runtime_error("m_window's pointer is empty");
  }
  auto scene = m_window.lock()->GetRenderLayer()->GetActiveScene();
  for (auto& widget : widgets) {
    ImGui::Begin(widget->GetWidgetName().c_str());
    widget->SetScene(scene);
    widget->Draw();
    ImGui::End();
  }
  DrawMenuBar();
}

void
DrawLayer::DrawMenuBar() {
  static bool isChangeWorkspace = false;
  static FileDialog workspaceFileDialog("*");

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      ImGui::MenuItem("choose a workspace", nullptr, &isChangeWorkspace);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  if (isChangeWorkspace) {
    workspaceFileDialog.Open(".");
    auto result = workspaceFileDialog.GetResult();
    LOG_IF(INFO, result.has_value()) << result.value();

    isChangeWorkspace = false;
  }
}

}  // namespace Marbas
