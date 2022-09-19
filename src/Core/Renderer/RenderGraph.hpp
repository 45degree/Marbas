#pragma once

#include <memory>

#include "Common/Common.hpp"
#include "Core/Renderer/DeferredRenderPass.hpp"
#include "Core/Renderer/ForwardRenderPass.hpp"
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
  RegisterDeferredRenderPassNode(const std::shared_ptr<DeferredRenderPass>& renderPassNode);

  void
  RegisterRenderTargetNode(const std::shared_ptr<RenderTargetNode>& renderTargetNode);

  void
  RegisterForwardRenderPassNode(const std::shared_ptr<ForwardRenderPass>& renderPassNode);

  // TODO: need to transparent sorting
  void
  Compile();

  void
  Execute(const std::shared_ptr<ResourceManager>& resourceManager,
          const std::shared_ptr<Scene>& scene);

  void
  ExecuteStaticRenderPass(const std::shared_ptr<ResourceManager>& resourceManager,
                          const std::shared_ptr<Scene>& scene);

  std::shared_ptr<RenderTargetNode>
  GetRenderTarget(const String& renderTargetName) const;

 private:
  std::unordered_map<String, int> m_deferredRenderPassMap;
  std::unordered_map<String, int> m_renderTargetMap;
  Vector<std::shared_ptr<DeferredRenderPass>> m_deferredRenderPassNodes;
  Vector<std::shared_ptr<RenderTargetNode>> m_renderTargetNode;
  Vector<std::shared_ptr<ForwardRenderPass>> m_forwardRendererPassNodes;

  Vector<int> m_renderOrder;
  Vector<int> m_staticRenderOrder;

  RHIFactory* m_rhiFactory;
  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas
