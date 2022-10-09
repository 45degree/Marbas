#pragma once

#include <imgui.h>

#include "Common/Common.hpp"
#include "Core/Layer/LayerBase.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

class ImguiLayer : public LayerBase {
 public:
  explicit ImguiLayer(const std::weak_ptr<Window>& window) : LayerBase(window) {}
  ~ImguiLayer() override;

 public:
  void
  OnAttach() override;

  void
  OnDetach() override;

  void
  OnBegin(const GlobalLayerInfo& info) override;

  void
  OnEnd(const GlobalLayerInfo& info) override;

  void
  OnResize(uint32_t width, uint32_t height) override;

  ImGuiContext*
  GetCurrentContext();

  void
  GetAllocateFunction(ImGuiMemAllocFunc* allocFunc, ImGuiMemFreeFunc* freeFunc, void** userData) {
    ImGui::GetAllocatorFunctions(allocFunc, freeFunc, userData);
  }

 private:
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  std::shared_ptr<ImguiInterface> m_imguiInterface = nullptr;
};

}  // namespace Marbas
