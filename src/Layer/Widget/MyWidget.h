#ifndef MARBARS_LAYER_WIDGET_MYWIDGET_H
#define MARBARS_LAYER_WIDGET_MYWIDGET_H

#include "Common.h"
#include "Layer/Widget/Widget.h"
#include "imgui.h"

namespace Marbas {

class MyWidget : public Widget {
public:
    MyWidget(): Widget("MyWidget") {}
    ~MyWidget() override = default;

    void Draw() override {
        ImGui::ShowDemoWindow();
    }

private:
    std::string title;
};

}  // namespace Marbas

#endif
