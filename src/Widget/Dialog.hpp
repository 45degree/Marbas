#ifndef MARBAS_WIDGET_DIALOG
#define MARBAS_WIDGET_DIALOG

#include "Widget/Widget.hpp"

namespace Marbas {

class Dialog : public Widget{
public:
    explicit Dialog(const String& dialogName): Widget(dialogName) {}
    ~Dialog() override = default;

public:
    virtual void Open() = 0;

};

}  // namespace Marbas

#endif
