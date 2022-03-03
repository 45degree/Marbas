#ifndef MARBARS_LAYER_WIDGET_WIDGET_H
#define MARBARS_LAYER_WIDGET_WIDGET_H

#include "Common.h"

namespace Marbas {

class Widget {
public:
    explicit Widget(const String& widgetName) : widgetName(widgetName) {}
    Widget(const Widget&) = default;
    virtual ~Widget();

public:

    [[nodiscard]] const String& GetWidgetName() const noexcept {
        return widgetName;
    }

    virtual void Draw() = 0;

protected:
    const String widgetName;
};

}  // namespace Marbas

#endif
