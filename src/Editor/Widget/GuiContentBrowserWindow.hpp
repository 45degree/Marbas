#pragma once

#include "GuiWindow.hpp"
#include "RHIFactory.hpp"

namespace Marbas::Gui {

class GuiContentBrowserWindow final : public GuiWindow {
 public:
  explicit GuiContentBrowserWindow(RHIFactory* rhiFactory);
  ~GuiContentBrowserWindow() override = default;

 public:
  void
  OnDraw() override;

  ImTextureID m_floderIcon;
  ImTextureID m_fileIcon;
  ImTextureID m_backIcon;

  RHIFactory* m_rhiFactory;
};

}  // namespace Marbas::Gui
