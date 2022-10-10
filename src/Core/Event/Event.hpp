#pragma once

#include <iostream>

#include "Common/Common.hpp"

namespace Marbas {

enum class EventType {
  MARBAS_MOUSE_PRESS_EVENT,
  MARBAS_MOUSE_RELEASE_EVENT,
  MARBAS_MOUSE_MOVE_EVENT,
  MARBAS_MOUSE_SCROLLED_EVENT,
  MARBAS_KEY_PRESS_EVENT,
  MARBAS_KEY_RELEASE_EVENT,
};

enum class EventCategory {
  MARBAS_MOUSE_EVENT,
  MARBAS_KEY_EVENT,
};

class Event {
 public:
  Event(EventType eventType, EventCategory eventCategory)
      : eventCategory(eventCategory), eventType(eventType) {}

  virtual ~Event() = default;

 public:
  [[nodiscard]] EventType
  GetEventType() const noexcept {
    return eventType;
  }

  [[nodiscard]] EventCategory
  GetEventCategory() const noexcept {
    return eventCategory;
  }

 protected:
  EventCategory eventCategory;
  EventType eventType;
};

class LayerBase;
class EventCollection {
 public:
  EventCollection() = default;
  ~EventCollection();

 public:
  void
  AddEvent(std::unique_ptr<Event>&& event) noexcept;

  void
  ClearEvent() {
    events.clear();
  }

  [[nodiscard]] size_t
  Size() const noexcept {
    return events.size();
  }

  void
  BroadcastEventFromLayer(LayerBase* layer);

 private:
  Vector<std::unique_ptr<Event>> events;
};

}  // namespace Marbas
