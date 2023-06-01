#pragma once

namespace Marbas::Gui {

class GuiBase {
 public:
  virtual ~GuiBase() = default;

 public:
  virtual void
  Draw() = 0;
};

}  // namespace Marbas::Gui
