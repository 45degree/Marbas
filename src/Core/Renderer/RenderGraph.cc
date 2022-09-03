#include "Core/Renderer/RenderGraph.hpp"

#include "RHI/OpenGL/OpenGLFrameBuffer.hpp"

namespace Marbas {

void
RenderGraph::RegisterDeferredRenderPassNode(
    const std::shared_ptr<DeferredRenderPass> &renderPassNode) {
  const String &passNodeName = renderPassNode->GetNodeName();
  DLOG_ASSERT(m_renderTargetMap.find(passNodeName) == m_renderTargetMap.cend())
      << FORMAT("the resource target node: {} has beed added into the render graph", passNodeName);

  const auto inputResource = renderPassNode->GetAllInputTargetName();
  const auto outputResources = renderPassNode->GetAllOutputTargetName();

  for (const auto &inputName : inputResource) {
    const auto &resource = m_renderTargetNode[m_renderTargetMap[inputName]];
    renderPassNode->SetInputTarget(resource);
  }

  for (const auto &outputName : outputResources) {
    const auto &resource = m_renderTargetNode[m_renderTargetMap[outputName]];
    renderPassNode->SetOutputTarget(resource);
  }
  m_deferredRenderPassNodes.push_back(renderPassNode);
  m_deferredRenderPassMap[passNodeName] = m_deferredRenderPassNodes.size() - 1;
}

void
RenderGraph::RegisterRenderTargetNode(const std::shared_ptr<RenderTargetNode> &renderTargetNode) {
  const String &targetNodeName = renderTargetNode->GetTargetName();
  DLOG_ASSERT(m_renderTargetMap.find(targetNodeName) == m_renderTargetMap.cend()) << FORMAT(
      "the resource target node: {} has beed added into the render graph", targetNodeName);

  int size = m_renderTargetNode.size();
  m_renderTargetNode.push_back(renderTargetNode);
  m_renderTargetMap.insert({targetNodeName, size});
}

void
RenderGraph::RegisterForwardRenderPassNode(
    const std::shared_ptr<ForwardRenderPass> &renderPassNode) {
  m_forwardRendererPassNodes.push_back(renderPassNode);
}

std::shared_ptr<RenderTargetNode>
RenderGraph::GetRenderTarget(const String &renderTargetName) const {
  DLOG_ASSERT(m_renderTargetMap.find(renderTargetName) != m_renderTargetMap.end())
      << FORMAT("can't find the target: {} in the render graph", renderTargetName);

  auto index = m_renderTargetMap.at(renderTargetName);
  return m_renderTargetNode[index];
}

void
RenderGraph::Compile() {
  for (auto &&renderPassNode : m_deferredRenderPassNodes) {
    renderPassNode->CreateFrameBuffer();
  }

  // TODO: need to test
  /**
   * topological sort
   */
  m_renderOrder.clear();

  // create the graph
  // in order to make a unique graph node id, all renderPass target node's id will add the
  // renderPass nodes count;
  int passCount = m_deferredRenderPassNodes.size();
  int targetCount = m_renderTargetNode.size();
  Vector<std::unordered_set<int>> graph(passCount + targetCount);
  Vector<int> degree(passCount + targetCount, 0);

  for (int i = 0; i < passCount; i++) {
    auto renderPass = m_deferredRenderPassNodes[i];
    auto outputs = renderPass->GetAllOutputTargetName();
    auto inputs = renderPass->GetAllInputTargetName();
    for (const auto &output : outputs) {
      auto id = m_renderTargetMap[output] + passCount;
      graph[i].insert(id);
    }
    for (const auto &input : inputs) {
      auto id = m_renderTargetMap[input] + passCount;
      graph[id].insert(i);
    }
  }

  for (int i = 0; i < graph.size(); i++) {
    for (const auto &id : graph[i]) {
      degree[id]++;
    }
  }

  // topological sort
  Vector<int> order;
  Vector<int> graphUsedflag(passCount + targetCount, 0);
  while (order.size() < passCount + targetCount) {
    for (int i = 0; i < degree.size(); i++) {
      if (degree[i] == 0 && !graphUsedflag[i]) {
        order.push_back(i);
        for (int id : graph[i]) {
          degree[id]--;
        }
        graph[i].clear();
        graphUsedflag[i] = 1;
      }
    }
  }

  // get render pass node
  for (int id : order) {
    if (id < passCount) {
      m_renderOrder.push_back(id);
    }
  }

  // set framebuffer for forward render
  for (auto &forwardRenderPass : m_forwardRendererPassNodes) {
    auto inputName = forwardRenderPass->GetInputTargetName();
    DLOG_ASSERT(m_deferredRenderPassMap.find(inputName) != m_deferredRenderPassMap.end())
        << FORMAT("can't find {} in all deferred render pass", inputName);
    auto id = m_deferredRenderPassMap[std::move(inputName)];
    auto frameBuffer = m_deferredRenderPassNodes[id]->GetFrameBuffer();
    forwardRenderPass->SetFrameBuffer(std::move(frameBuffer));
  }
}

void
RenderGraph::Execute(const std::shared_ptr<ResourceManager> &resourceManager,
                     const std::shared_ptr<Scene> &scene) {
  for (int i = 0; i < m_renderOrder.size(); i++) {
    m_deferredRenderPassNodes[m_renderOrder[i]]->Execute(scene.get(), resourceManager.get());
  }

  // execute forward rendering
  for (const auto &forwardRenderPass : m_forwardRendererPassNodes) {
    forwardRenderPass->Execute(scene.get(), resourceManager.get());
  }
}

}  // namespace Marbas
