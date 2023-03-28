#include "Widget.hpp"

namespace Marbas {

Widget::Widget(const String& widgetName, RHIFactory* rhiFactory) : m_widgetName(widgetName), m_rhiFactory(rhiFactory) {}

Widget::~Widget() = default;

};  // namespace Marbas
