#pragma once

#include "Core/Camera.hpp"
#include "Core/Layer/LayerBase.hpp"
#include "Core/Renderer/RenderGraph.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHI/RHI.hpp"
#include "Resource/ResourceManager.hpp"

namespace Marbas {

class RenderLayer : public LayerBase {
 public:
  RenderLayer(int width, int height, const std::weak_ptr<Window>& window);
  ~RenderLayer() override;

 public:
  void
  OnAttach() override;

  void
  OnDetach() override;

  void
  OnUpdate() override;

  void
  OnMouseMove(const MouseMoveEvent& e) override;

  void
  OnMouseScrolled(const MouseScrolledEvent& e) override;

 public:
  [[nodiscard]] std::shared_ptr<Scene>
  GetActiveScene() const {
    if (m_activeSceneIndex >= 0 && m_activeSceneIndex < m_allScene.size()) {
      return m_allScene[m_activeSceneIndex];
    }

    LOG(WARNING) << FORMAT("the scene's count is {}, but the index is {}, this will return nullptr",
                           m_allScene.size(), m_activeSceneIndex);
    return nullptr;
  }

  std::shared_ptr<Texture2D>
  GetRenderResult();

 private:
  RHIFactory* m_rhiFactory;
  uint32_t m_height;
  uint32_t m_width;

  Vector<std::shared_ptr<Scene>> m_allScene;
  int m_activeSceneIndex = 0;

  std::shared_ptr<RenderGraph> m_renderGraph;

 private:
  float m_mouseLastX;
  float m_mouseLastY;
};

}  // namespace Marbas
