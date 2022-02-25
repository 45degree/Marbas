#ifndef MARBARS_LAYER_IMGUI_LAYER_H
#define MARBARS_LAYER_IMGUI_LAYER_H

#include "Layer/Layer.h"
#include "Common.h"
#include "Core/Window.h"
#include <imgui.h>

namespace Marbas {

class ImguiLayer : public Layer {
public:
    explicit ImguiLayer(const Window* window) : Layer("ImguiLayer"), window(window) {}
    virtual ~ImguiLayer();

public:
    void OnAttach() override;
    void OnDetach() override;

    void OnBegin() override;
    void OnEnd() override;

private:
    const Window* window;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

}

#endif
