#ifndef MARBARS_LAYER_IMGUI_LAYER_H
#define MARBARS_LAYER_IMGUI_LAYER_H

#include <imgui.h>

#include "Common.hpp"
#include "Layer/LayerBase.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

class ImguiLayer : public LayerBase {
 public:
  explicit ImguiLayer(const Window* window) : LayerBase(window) {}
  ~ImguiLayer() override;

 public:
  void OnAttach() override;
  void OnDetach() override;

  void OnBegin() override;
  void OnEnd() override;

 private:
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  RHIFactory* m_rhiFactory = nullptr;

  std::unique_ptr<Viewport> m_viewport = nullptr;
};

}  // namespace Marbas

#endif
