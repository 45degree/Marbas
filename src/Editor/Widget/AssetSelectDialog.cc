#include "AssetSelectDialog.hpp"

#include <imgui.h>

#include <nameof.hpp>

namespace Marbas {

void
AssetSelectDialog::Open() {
  ImGui::OpenPopup(NAMEOF_TYPE(AssetSelectDialog).data());

  if (ImGui::BeginPopupModal(NAMEOF_TYPE(AssetSelectDialog).data())) {
    ImGui::EndPopup();
  }
}

}  // namespace Marbas
