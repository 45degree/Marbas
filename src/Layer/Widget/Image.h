#ifndef MARBARS_LAYER_WIDGET_IMAGE_H
#define MARBARS_LAYER_WIDGET_IMAGE_H

#include "Layer/Widget/Widget.h"
#include "RHI/RHI.h"
#include "imgui.h"
#include "Common.h"

namespace Marbas {

class Image : public Widget {
public:
    explicit Image(ImTextureID textureId = nullptr, const Viewport* viewport = nullptr):
        Widget("Image"),
        texture(textureId),
        viewport(viewport)
    {
    }

    ~Image() override = default;

    void Draw() override {
        auto [x, y, width, height] = viewport->GetViewport();
        imageSize = ImGui::GetContentRegionAvail();

        float ratio;
        if(width > height) {
            ratio = imageSize.x / static_cast<float>(width);
            imageSize.y = ratio * static_cast<float>(height);
        }
        else {
            ratio = imageSize.y / static_cast<float>(height);
            imageSize.x = ratio * static_cast<float>(width);
        }

        ImGui::Image(texture, imageSize, ImVec2(0, 1), ImVec2(1, 0));
    }

    void SetViewport(const Viewport* viewport) noexcept {
        this->viewport = viewport;
    }

    void ChangeTexture(ImTextureID textureId) {
        texture = textureId;
    }

    [[nodiscard]] const ImVec2 GetImageSize() const noexcept {
        return imageSize;
    }

private:
    ImTextureID texture;
    ImVec2 imageSize;
    const Viewport* viewport;
};

}  // namespace Marbas

#endif
