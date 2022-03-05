#ifndef MARBARS_EVENT_EVENT_H
#define MARBARS_EVENT_EVENT_H

#include <iostream>
#include "Common.h"

namespace Marbas {

enum class EventType {
    MARBAS_MOUSE_PRESS_EVENT,
    MARBAS_MOUSE_RELEASE_EVENT,
    MARBAS_MOUSE_MOVE_EVENT,
    MARBAS_MOUSE_SCROLLED_EVENT,
};

enum class EventCategory {
    MARBARS_MOUSE_EVENT,
};

class Event {
public:
    Event(EventType eventType, EventCategory eventCategory):
        eventCategory(eventCategory),
        eventType(eventType)
    {}

    virtual ~Event() = default;

public:
    [[nodiscard]] EventType GetEventType() const noexcept { return eventType; }

    [[nodiscard]] EventCategory GetEventCategory() const noexcept { return eventCategory; }

protected:
    EventCategory eventCategory;
    EventType eventType;
};

class Layer;
class EventCollection {
public:
    EventCollection() = default;
    ~EventCollection();

public:
    void AddEvent(std::unique_ptr<Event>&& event) noexcept;

    void ClearEvent() {
        events.clear();
    }

    [[nodiscard]] size_t Size() const noexcept {
        return events.size();
    }

    void BroadcastEventFromLayer(Layer* layer);

private:
    Vector<std::unique_ptr<Event>> events;
};

}  // namespace Marbas

#endif
