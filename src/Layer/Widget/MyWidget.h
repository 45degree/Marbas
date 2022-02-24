#ifndef MARBARS_LAYER_WIDGET_MYWIDGET_H
#define MARBARS_LAYER_WIDGET_MYWIDGET_H

#include "Layer/Widget/Widget.h"
#include "imgui.h"
#include "glog/logging.h"

namespace Marbas {

class MyWidget : public Widget {
public:
    MyWidget(const std::string& title): title(title){}
    virtual ~MyWidget() {}

    void Draw() override {
        ImGui::ShowDemoWindow();
    }

private:
    std::string title;
};

}

#endif
