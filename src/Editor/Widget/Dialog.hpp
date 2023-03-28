#pragma once

#include "Widget.hpp"

namespace Marbas {

class Dialog : public Widget {
 public:
  explicit Dialog(RHIFactory* rhiFactory, const String& dialogName) : Widget(dialogName, rhiFactory) {}
  ~Dialog() override = default;

 public:
  virtual void
  Open() = 0;
};

}  // namespace Marbas
