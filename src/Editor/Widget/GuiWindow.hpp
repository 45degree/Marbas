#pragma once

#include <imgui/imgui.h>

#include <string>

#include "GuiWidget.hpp"

namespace Marbas::Gui {

class GuiWindow : public GuiWidget {
 public:
  GuiWindow(const std::string& titleName) : GuiWidget(titleName) {}
  ~GuiWindow() override = default;

  void
  Draw() override final {
    if (ImGui::Begin(m_name.c_str(), &m_isOpen)) {
      OnDraw();
    }
    ImGui::End();
  }

 private:
  bool m_isOpen = false;
};

}  // namespace Marbas::Gui
