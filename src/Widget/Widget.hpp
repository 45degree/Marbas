#ifndef MARBARS_WIDGET_WIDGET_H
#define MARBARS_WIDGET_WIDGET_H

#include "Common.hpp"
#include "Core/Scene.hpp"

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

    void SetScene(Scene* scene) noexcept {
        m_scene = scene;
    }

protected:
    const String m_widgetName;
    Scene* m_scene;
};

}  // namespace Marbas

#endif
