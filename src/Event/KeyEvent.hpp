#ifndef MARBAS_EVENT_KEYEVENT_H
#define MARBAS_EVENT_KEYEVENT_H

#include "Event/Event.hpp"
#include "Common.hpp"

namespace Marbas {

class KeyEvent : public Event {
public:
    explicit KeyEvent(EventType eventType) :
        Event(eventType, EventCategory::MARBAS_KEY_EVENT)
    {}

    ~KeyEvent() override = default;

    KeyEvent(const KeyEvent&) = default;
    KeyEvent& operator=(const KeyEvent&) = default;

public:
    void SetKey(int key) noexcept { m_key = key; }

    [[nodiscard]] int GetKey() const noexcept { return m_key; }

private:
    int m_key;
};

}  // namespace Marbas

#endif
