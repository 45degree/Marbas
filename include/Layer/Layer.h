#ifndef MARBARS_LAYER_LAYER_H
#define MARBARS_LAYER_LAYER_H

#include "Event/Event.h"
#include <folly/FBString.h>

namespace Marbas::Layer {

class Layer {
    using Event = Marbas::Event::Event;

public:
    Layer(const folly::fbstring& layerName) : layerName(layerName) {}
    virtual ~Layer() = default;

public:

    virtual void OnAttach() {};

    virtual void OnDetach() {};

protected:
    virtual void OnUpdate() {};

public:
    virtual void OnMouseClick(const Event& e) {}
    virtual void OnKeyPress(const Event& e) {}

protected:
    folly::fbstring layerName;
};

}

#endif
