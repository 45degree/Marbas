#ifndef MARBARS_WIDGET_IMAGE_H
#define MARBARS_WIDGET_IMAGE_H

#include "Layer/RenderLayer.hpp"
#include "Widget/Widget.hpp"

#include <imgui.h>

namespace Marbas {

class Image : public Widget {
public:
    explicit Image(const String& title = "Image"):
        Widget(title)
    {}

    ~Image() override = default;

    void Draw() override;

    [[nodiscard]] const ImVec2 GetImageSize() const noexcept {
        return imageSize;
    }

private:
    ImVec2 imageSize;
};

}  // namespace Marbas

#endif
