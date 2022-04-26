#ifndef MARBARS_LAYER_LAYERBASE_H
#define MARBARS_LAYER_LAYERBASE_H

#include "Event/Event.hpp"
#include "Event/MouseEvent.hpp"
#include "Event/KeyEvent.hpp"
#include "Common.hpp"

namespace Marbas {

class Window;
class LayerBase {
public:
    explicit LayerBase(const Window* window);

    virtual ~LayerBase();

public:
    void AddNextLayer(LayerBase* nextLayer) {
        m_nextLayer = nextLayer;
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
    const Window* m_window = nullptr;
    LayerBase* m_nextLayer = nullptr;
};

}  // namespace Marbas

#endif
