#ifndef MARBARS_LAYER_WIDGET_MYWIDGET_H
#define MARBARS_LAYER_WIDGET_MYWIDGET_H

#include "Common.h"
#include "Layer/Widget/Widget.h"
#include "imgui.h"

namespace Marbas {

class MyWidget : public Widget {
public:
    explicit MyWidget(const std::string& title): title(title){}
    virtual ~MyWidget() {}

    void Draw() override {
        ImGui::ShowDemoWindow();
    }

private:
    std::string title;
};

}

#endif
