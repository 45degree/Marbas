#ifndef MARBARS_LAYER_DRAW_H
#define MARBARS_LAYER_DRAW_H

#include "Layer/Layer.h"
#include "Widget/Widget.h"
#include "Common.h"

namespace Marbas {

class DrawLayer : public Layer {
public:
    DrawLayer() : Layer("DrawLayer") {}
    ~DrawLayer() override;

public:
    void OnUpdate() override;

    void AddWidget(std::unique_ptr<Widget>&& widget) noexcept {
        widgets.push_back(std::move(widget));
    }

private:
    Vector<std::unique_ptr<Widget>> widgets;
};

}  // namespace Marbas

#endif
