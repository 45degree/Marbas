#include "Layer/DrawLayer.h"
#include "glog/logging.h"

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
