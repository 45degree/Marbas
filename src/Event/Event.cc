#include "Event/Event.h"
#include "Layer/LayerBase.h"

namespace Marbas {

EventCollection::~EventCollection() = default;

void EventCollection::AddEvent(std::unique_ptr<Event>&& event) noexcept {
    events.push_back(std::move(event));
}

void EventCollection::BroadcastEventFromLayer(LayerBase* layer) {
    for(auto& event : events) {
        const Event& e = *event;
        layer->BroadcastEvent(e);
    }
}


}  // namespace Marbas
