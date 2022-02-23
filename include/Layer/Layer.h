#ifndef MARBARS_LAYER_LAYER_H
#define MARBARS_LAYER_LAYER_H

#include "Event/Event.h"
#include "Event/MouseEvent.h"

#include <folly/FBString.h>

namespace Marbas {

class Layer {
public:
    Layer(const folly::fbstring& layerName) :
        layerName(layerName)
    {}

    virtual ~Layer() = 0;

public:
    void AddNextLayer(std::unique_ptr<Layer>&& nextLayer) {
        this->nextLayer = std::move(nextLayer);
    }

public:
    void Attach();

    void Detach();

    void Update();

    void Begin();

    void End();

    void BroadcastEvent(const Event&);

public:
    virtual void OnAttach() {};

    virtual void OnDetach() {};

    virtual void OnUpdate() {};

    virtual void OnBegin() {};

    virtual void OnEnd() {};

    void EventDistribution(const Event& event);

public:
    virtual void OnMousePress(const MousePressEvent& e) {}
    virtual void OnMouseRelease(const MouseReleaseEvent& e) {}
    virtual void OnKeyPress(const Event& e) {}
    virtual void OnMouseMove(const MouseMoveEvent& e) {};
    virtual void OnMouseScrolled(const MouseScrolledEvent& e) {};

protected:
    folly::fbstring layerName;

    std::unique_ptr<Layer> nextLayer = nullptr;
};

}

#endif
