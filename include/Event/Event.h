#ifndef MARBARS_EVENT_EVENT_H
#define MARBARS_EVENT_EVENT_H

#include <folly/FBVector.h>
#include <iostream>

namespace Marbas {

enum class EventType {
    MOUSE_PRESS_EVENT,
    MOUSE_RELEASE_EVENT,
    MOUSE_MOVE_EVENT,
    MOUSE_SCROLLED_EVENT,
};

enum class EventCategory {
    MOUSE_EVENT,
};

class Event {
public:
    Event(EventType eventType, EventCategory eventCategory):
        eventType(eventType),
        eventCategory(eventCategory)
    {}

    virtual ~Event() = default;

public:
    EventType GetEventType() const noexcept { return eventType; }

    EventCategory GetEventCategory() const noexcept { return eventCategory; }

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

    size_t Size() const noexcept {
        return events.size();
    }

    void BroadcastEventFromLayer(Layer* layer);

private:
    std::vector<std::unique_ptr<Event>> events;
};

}

#endif
