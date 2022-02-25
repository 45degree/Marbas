#ifndef MARBARS_EVENT_MOUSEEVENT_H
#define MARBARS_EVENT_MOUSEEVENT_H

#include "Event/Event.h"
#include "tuple"

namespace Marbas {

class MouseEvent : public Event {
public:
    explicit MouseEvent(EventType eventType) : Event(eventType, EventCategory::MOUSE_EVENT) {}
    virtual ~MouseEvent() = default;

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
    virtual ~MouseButtonEvent() = default;

public:

    int GetButton() const noexcept {
        return button;
    }

protected:
    int button;
};

class MousePressEvent : public MouseButtonEvent {
public:
    explicit MousePressEvent(int button) :
        MouseButtonEvent(EventType::MOUSE_PRESS_EVENT, button)
    {}
    virtual ~MousePressEvent() = default;
};

class MouseReleaseEvent : public MouseButtonEvent {
public:
    explicit MouseReleaseEvent(int button) :
        MouseButtonEvent(EventType::MOUSE_RELEASE_EVENT, button)
    {}
    virtual ~MouseReleaseEvent() = default;
};

class MouseMoveEvent : public MouseEvent {
public:
    MouseMoveEvent(): MouseEvent(EventType::MOUSE_MOVE_EVENT)
    {}
    virtual ~MouseMoveEvent() = default;
};

class MouseScrolledEvent : public MouseEvent {
public:
    MouseScrolledEvent(double xoffset, double yoffset) :
        MouseEvent(EventType::MOUSE_SCROLLED_EVENT),
        xoffset(xoffset),
        yoffset(yoffset)
    {}

    virtual ~MouseScrolledEvent() = default;

public:
    double GetXOffset() const noexcept {
        return xoffset;
    }

    double GetYOffset() const noexcept {
        return yoffset;
    }

private:
    double xoffset;
    double yoffset;
};

};

#endif
