#pragma once

#include "Core/Layer/LayerBase.hpp"

namespace Marbas {

class DockspaceLayer : public LayerBase {
 public:
  explicit DockspaceLayer(const std::weak_ptr<Window>& window) : LayerBase(window) {}
  ~DockspaceLayer() override;

 public:
  void
  OnAttach() override;

  void
  OnDetach() override;

  void
  OnUpdate(const GlobalLayerInfo&) override;

  void
  OnBegin(const GlobalLayerInfo&) override;

  void
  OnEnd(const GlobalLayerInfo&) override;
};

}  // namespace Marbas
