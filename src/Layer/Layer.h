#ifndef MARBARS_LAYER_LAYER_H
#define MARBARS_LAYER_LAYER_H

#include "Event/Event.h"
#include "Event/MouseEvent.h"
#include "Event/KeyEvent.h"
#include "Common.h"

namespace Marbas {

class Layer {
public:
    explicit Layer(const folly::fbstring& layerName) :
        layerName(layerName)
    {}

    virtual ~Layer() = 0;

public:
    void AddNextLayer(std::unique_ptr<Layer>&& nextLayer) {
        this->nextLayer = std::move(nextLayer);
    }

    const String& GetLayerName() {
        return layerName;
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
    virtual void OnKeyPress(const KeyEvent& e) {}
    virtual void OnKeyRelase(const KeyEvent& e) {}
    virtual void OnMouseMove(const MouseMoveEvent& e) {};
    virtual void OnMouseScrolled(const MouseScrolledEvent& e) {};

protected:
    String layerName;
    std::unique_ptr<Layer> nextLayer = nullptr;
};

}  // namespace Marbas

#endif
