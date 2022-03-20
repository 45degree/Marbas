#ifndef MARBARS_EVENT_MOUSEEVENT_H
#define MARBARS_EVENT_MOUSEEVENT_H

#include "Event/Event.h"
#include "tuple"

namespace Marbas {

class MouseEvent : public Event {
public:
    explicit MouseEvent(EventType eventType) :
        Event(eventType, EventCategory::MARBAS_MOUSE_EVENT)
    {}
    ~MouseEvent() override = default;

    MouseEvent(const MouseEvent&) = default;
    MouseEvent& operator=(const MouseEvent&) = default;

public:

    void SetPos(double x, double y) noexcept {
        this->x = x;
        this->y = y;
    }

    std::tuple<double, double> GetPos() const noexcept {
        return std::make_tuple(x, y);
    }

protected:
    double x = 0.0;
    double y = 0.0;
};

class MouseButtonEvent : public MouseEvent {
public:
    MouseButtonEvent(EventType eventType, int button):
        MouseEvent(eventType),
        button(button)
    {}
    ~MouseButtonEvent() override = default;

public:

    [[nodiscard]] int GetButton() const noexcept {
        return button;
    }

protected:
    int button;
};

class MousePressEvent : public MouseButtonEvent {
public:
    explicit MousePressEvent(int button) :
        MouseButtonEvent(EventType::MARBAS_MOUSE_PRESS_EVENT, button)
    {}
    ~MousePressEvent() override = default;
};

class MouseReleaseEvent : public MouseButtonEvent {
public:
    explicit MouseReleaseEvent(int button) :
        MouseButtonEvent(EventType::MARBAS_MOUSE_RELEASE_EVENT, button)
    {}
    ~MouseReleaseEvent() override = default;
};

class MouseMoveEvent : public MouseEvent {
public:
    MouseMoveEvent(): MouseEvent(EventType::MARBAS_MOUSE_MOVE_EVENT)
    {}
    ~MouseMoveEvent() override = default;
};

class MouseScrolledEvent : public MouseEvent {
public:
    MouseScrolledEvent(double xoffset, double yoffset) :
        MouseEvent(EventType::MARBAS_MOUSE_SCROLLED_EVENT),
        xoffset(xoffset),
        yoffset(yoffset)
    {}

    ~MouseScrolledEvent() override = default;

public:
    [[nodiscard]] double GetXOffset() const noexcept {
        return xoffset;
    }

    [[nodiscard]] double GetYOffset() const noexcept {
        return yoffset;
    }

private:
    double xoffset;
    double yoffset;
};

}  // namespace Marbas

#endif
