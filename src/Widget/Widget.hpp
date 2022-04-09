#ifndef MARBARS_WIDGET_WIDGET_H
#define MARBARS_WIDGET_WIDGET_H

#include "Common.hpp"

namespace Marbas {

class Widget {
public:
    explicit Widget(const String& widgetName) : m_widgetName(widgetName) {}
    Widget(const Widget&) = default;
    virtual ~Widget();

public:

    [[nodiscard]] const String& GetWidgetName() const noexcept {
        return m_widgetName;
    }

    virtual void Draw() = 0;

protected:
    const String m_widgetName;
};

}  // namespace Marbas

#endif
