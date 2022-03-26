#ifndef MARBARS_WIDGET_WIDGET_H
#define MARBARS_WIDGET_WIDGET_H

#include "Common.h"

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

    [[nodiscard]] bool IsShow() const noexcept {
        return m_isShow;
    }

    void SetShow(bool isShow) noexcept {
        m_isShow = isShow;
    }

protected:
    bool m_isShow = true;
    const String m_widgetName;
};

}  // namespace Marbas

#endif
