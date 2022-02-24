#ifndef MARBARS_LAYER_WIDGET_WIDGET_H
#define MARBARS_LAYER_WIDGET_WIDGET_H

namespace Marbas {

class Widget {
public:
    Widget() {}
    Widget(const Widget&) = default;
    Widget& operator=(const Widget&) = default;
    virtual ~Widget();

    virtual void Draw() = 0;
};

};

#endif
