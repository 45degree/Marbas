#include "Common.h"
#include "Layer/DrawLayer.h"
#include "imgui.h"

namespace Marbas {

DrawLayer::~DrawLayer() {
    LOG(INFO) << "release draw layer";
}

void DrawLayer::OnUpdate() {
    for(auto& widget : widgets) {
        ImGui::Begin(widget->GetWidgetName().c_str());
        widget->Draw();
        ImGui::End();
    }
}
}  // namespace Marbas
