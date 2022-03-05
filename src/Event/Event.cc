#include "Event/Event.h"
#include "Layer/Layer.h"

namespace Marbas {

EventCollection::~EventCollection() = default;

void EventCollection::AddEvent(std::unique_ptr<Event>&& event) noexcept {
    events.push_back(std::move(event));
}

void EventCollection::BroadcastEventFromLayer(Layer* layer) {
    for(int i = 0; i < events.size(); i++) {
        const Event& e = *events[i];
        layer->BroadcastEvent(e);
    }
}


}  // namespace Marbas
