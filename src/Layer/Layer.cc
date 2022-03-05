#include "Layer/Layer.h"

namespace Marbas {

Layer::~Layer() = default;

void Layer::Attach() {
    OnAttach();

    if(nextLayer != nullptr) {
        nextLayer->Attach();
    }
}

void Layer::Detach() {
    if(nextLayer != nullptr) {
        nextLayer->Detach();
    }
    OnDetach();
}

void Layer::Begin() {
    OnBegin();

    if(nextLayer != nullptr) {
        nextLayer->Begin();
    }
}

void Layer::End() {
    if(nextLayer != nullptr) {
        nextLayer->End();
    }

    OnEnd();
}

void Layer::Update() {
    OnUpdate();

    if(nextLayer != nullptr) {
        nextLayer->Update();
    }
}

void Layer::BroadcastEvent(const Event& e) {
    EventDistribution(e);

    if(nextLayer != nullptr) {
        nextLayer->BroadcastEvent(e);
    }
}

void Layer::EventDistribution(const Event& event) {
    switch(event.GetEventType()) {
    case EventType::MARBAS_MOUSE_PRESS_EVENT:
        OnMousePress(static_cast<const MousePressEvent&>(event));
        break;
    case EventType::MARBAS_MOUSE_MOVE_EVENT:
        OnMouseMove(static_cast<const MouseMoveEvent&>(event));
        break;
    case EventType::MARBAS_MOUSE_RELEASE_EVENT:
        OnMouseRelease(static_cast<const MouseReleaseEvent&>(event));
        break;
    case EventType::MARBAS_MOUSE_SCROLLED_EVENT:
        OnMouseScrolled(static_cast<const MouseScrolledEvent&>(event));
        break;
    }
}

}  // namespace Marbas

