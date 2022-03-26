#ifndef MARBARS_WIDGET_IMAGE_H
#define MARBARS_WIDGET_IMAGE_H

#include "Layer/RenderLayer.h"
#include "Widget/Widget.h"
#include "RHI/RHI.h"
#include "imgui.h"
#include "Common.h"

namespace Marbas {

class Image : public Widget {
public:
    explicit Image(RenderLayer* renderLayer, Viewport* viewport = nullptr):
        Widget("Image"),
        m_renderLayer(renderLayer),
        viewport(viewport)
    {
    }

    ~Image() override = default;

    void Draw() override {
        auto [x, y, width, height] = viewport->GetViewport();
        imageSize = ImGui::GetContentRegionAvail();

        auto textureId = const_cast<ImTextureID>(m_renderLayer->GetFrameBufferTexture());

        // float ratio;
        // if(width > height) {
        //     ratio = imageSize.x / static_cast<float>(width);
        //     imageSize.y = ratio * static_cast<float>(height);
        // }
        // else {
        //     ratio = imageSize.y / static_cast<float>(height);
        //     imageSize.x = ratio * static_cast<float>(width);
        // }

        ImGui::Image(textureId, imageSize, ImVec2(0, 1), ImVec2(1, 0));

        viewport->SetViewport(x, y, static_cast<int>(imageSize.x), static_cast<int>(imageSize.y));
    }

    void SetViewport(Viewport* viewport) noexcept {
        this->viewport = viewport;
    }

    [[nodiscard]] const ImVec2 GetImageSize() const noexcept {
        return imageSize;
    }

private:
    RenderLayer* m_renderLayer;
    ImVec2 imageSize;
    Viewport* viewport;
};

}  // namespace Marbas

#endif
