#ifndef MARBARS_LAYER_DRAW_H
#define MARBARS_LAYER_DRAW_H

#include "Layer/Layer.h"
#include "Layer/Widget/Widget.h"
#include "folly/FBVector.h"

namespace Marbas {

class DrawLayer : public Layer {
public:
    DrawLayer() : Layer("DrawLayer") {}
    virtual ~DrawLayer();

public:
    void OnUpdate() override;

    void AddWidget(std::unique_ptr<Widget>&& widget) noexcept {
        widgets.push_back(std::move(widget));
    }

private:
    folly::fbvector<std::unique_ptr<Widget>> widgets;
};

};

#endif
