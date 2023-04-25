#pragma once

#include "Widget.hpp"

namespace Marbas {

class ContentBrowser final : public Widget {
 public:
  explicit ContentBrowser(RHIFactory* rhiFactory);
  ~ContentBrowser() override = default;

 public:
  void
  Draw() override;

  ImTextureID m_floderIcon;
  ImTextureID m_fileIcon;
  ImTextureID m_backIcon;
};

}  // namespace Marbas
