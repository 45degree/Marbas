#ifndef MARBARS_LAYER_WIDGET_IMAGE_H
#define MARBARS_LAYER_WIDGET_IMAGE_H

#include "Layer/Widget/Widget.h"
#include "imgui.h"
#include "Common.h"

namespace Marbas {

class Image : public Widget {
public:
    explicit Image(ImTextureID textureId = nullptr):
        Widget("Image"),
        texture(textureId)
    {}

    ~Image() override = default;

    void Draw() override {
        auto windowsSize = ImGui::GetWindowSize();
        ImGui::Image(texture, windowsSize);
    }

    void ChangeTexture(ImTextureID textureId) {
        texture = textureId;
    }

private:
    ImTextureID texture;
};

}  // namespace Marbas

#endif
