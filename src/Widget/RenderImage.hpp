#ifndef MARBARS_WIDGET_IMAGE_H
#define MARBARS_WIDGET_IMAGE_H

#include "Layer/RenderLayer.hpp"
#include "Widget/Widget.hpp"

#include <imgui.h>

namespace Marbas {

class RenderImage : public Widget {
public:
    explicit RenderImage(const String& title = "Image"):
        Widget(title)
    {}

    ~RenderImage() override = default;

    void Draw() override;

    [[nodiscard]] const ImVec2 GetImageSize() const noexcept {
        return imageSize;
    }

    void SetSelectedSceneNode(Mesh mesh) {
        m_selectedMesh = mesh;
    }

private:
    std::optional<Mesh> m_selectedMesh;
    ImVec2 imageSize;
};

}  // namespace Marbas

#endif
