#include "Common.h"
#include "Layer/DrawLayer.h"

namespace Marbas {

DrawLayer::~DrawLayer() {
    LOG(INFO) << "release draw layer";
}

void DrawLayer::OnUpdate() {
    for(auto& widget : widgets) {
        widget->Draw();
    }
}
}
