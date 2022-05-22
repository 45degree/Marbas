#pragma once

#include <memory>

#include "Common/Common.hpp"
#include "Core/Renderer/RenderPassNode.hpp"
#include "Core/Renderer/RenderTargetNode.hpp"
#include "Core/Scene/Scene.hpp"
#include "Resource/ResourceManager.hpp"

namespace Marbas {

class RenderGraph final {
 public:
  RenderGraph(uint32_t height, uint32_t width, RHIFactory* rhiFactory)
      : m_height(height), m_width(width), m_rhiFactory(rhiFactory) {}

 public:
  void
  RegisterRenderPassNode(const std::shared_ptr<RenderPassNode>& renderPassNode);

  void
  RegisterRenderTargetNode(const std::shared_ptr<RenderTargetNode>& renderTargetNode);

  void
  Compile();

  void
  Execute(const std::shared_ptr<ResourceManager>& resourceManager,
          const std::shared_ptr<Scene>& scene);

  std::shared_ptr<RenderTargetNode>
  GetRenderTarget(const String& renderTargetName) const;

 private:
  std::unordered_map<String, int> m_renderPassMap;
  std::unordered_map<String, int> m_renderTargetMap;
  Vector<std::shared_ptr<RenderPassNode>> m_renderPassNodes;
  Vector<std::shared_ptr<RenderTargetNode>> m_renderTargetNode;
  Vector<int> m_renderOrder;

  RHIFactory* m_rhiFactory;
  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas
