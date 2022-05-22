#include "Core/Renderer/RenderGraph.hpp"

namespace Marbas {

void
RenderGraph::RegisterRenderPassNode(const std::shared_ptr<RenderPassNode> &renderPassNode) {
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
  m_renderPassNodes.push_back(renderPassNode);
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

std::shared_ptr<RenderTargetNode>
RenderGraph::GetRenderTarget(const String &renderTargetName) const {
  DLOG_ASSERT(m_renderTargetMap.find(renderTargetName) != m_renderTargetMap.end())
      << FORMAT("can't find the target: {} in the render graph", renderTargetName);

  auto index = m_renderTargetMap.at(renderTargetName);
  return m_renderTargetNode[index];
}

void
RenderGraph::Compile() {
  for (auto &&renderPassNode : m_renderPassNodes) {
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
  int passCount = m_renderPassNodes.size();
  int targetCount = m_renderTargetNode.size();
  Vector<std::unordered_set<int>> graph(passCount + targetCount);
  Vector<int> degree(passCount + targetCount, 0);

  for (int i = 0; i < passCount; i++) {
    auto renderPass = m_renderPassNodes[i];
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
    for (auto &id : graph[i]) {
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
      }
    }
  }

  // get render pass node
  for (int id : order) {
    if (id < passCount) {
      m_renderOrder.push_back(id);
    }
  }
}

void
RenderGraph::Execute(const std::shared_ptr<ResourceManager> &resourceManager,
                     const std::shared_ptr<Scene> &scene) {
  for (int i = 0; i < m_renderOrder.size(); i++) {
    m_renderPassNodes[m_renderOrder[i]]->Execute(scene, resourceManager);
  }
}

}  // namespace Marbas
