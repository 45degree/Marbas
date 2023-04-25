#include "RenderGraph.hpp"

#include <glog/logging.h>

#include <cmath>

#include "RenderGraphPass.hpp"

namespace Marbas {

void
RenderGraph::Compile() {
  // create all resource
  for (auto& res : m_resourceManager->m_graphTexture) {
    res.Create();
  }

  // create render resource for all pass
  for (auto& pass : m_passes) {
    pass->Initialize(this);
  }

  // create semaphores
  for (auto* semaphore : m_semaphores) {
    m_rhiFactory->DestroyGPUSemaphore(semaphore);
  }
  m_semaphores.clear();

  for (int i = 0; i < static_cast<int>(m_passes.size()) - 1; i++) {
    m_semaphores.push_back(m_rhiFactory->CreateGPUSemaphore());
  }

  DLOG(INFO) << "compile render graph successful";
}

void
RenderGraph::Execute(Semaphore* waitSemaphore, Semaphore* signalSemaphore, Fence* fence) {
  // find all enabled pass
  std::vector<int> enablePassesIndex;
  for (int i = 0; i < m_passes.size(); i++) {
    if (m_passes[i]->IsEnable()) {
      enablePassesIndex.push_back(i);
    }
  }

  // prepare the semaphores
  std::vector<Semaphore*> enablePassSemaphore;
  int needSemaphoresCount = static_cast<int>(enablePassesIndex.size()) - 1;
  if (needSemaphoresCount > 0) {
    enablePassSemaphore = {m_semaphores.begin(), m_semaphores.begin() + needSemaphoresCount};
  }
  enablePassSemaphore.insert(enablePassSemaphore.begin(), waitSemaphore);
  enablePassSemaphore.insert(enablePassSemaphore.end(), signalSemaphore);

  // record the command
  for (int& enablePass : enablePassesIndex) {
    m_passes[enablePass]->Execute(this);
  }

  // execute the command
  for (int i = 0; i < static_cast<int>(enablePassesIndex.size() - 1); i++) {
    int currentIndex = enablePassesIndex[i];
    m_passes[currentIndex]->Submit(enablePassSemaphore[i], enablePassSemaphore[i + 1], nullptr);
  }

  // execute the last command
  if (!enablePassesIndex.empty()) {
    auto index = static_cast<int>(enablePassesIndex.size()) - 1;
    int currentPass = enablePassesIndex[index];
    m_passes[currentPass]->Submit(enablePassSemaphore[index], enablePassSemaphore[index + 1], fence);
  }
}

void
RenderGraph::ExecuteAlone(const StringView& passName, Semaphore* waitSemaphore, Semaphore* signalSemaphore,
                          Fence* fence) {
  auto iter = std::find_if(m_passes.begin(), m_passes.end(), [&](auto* pass) { return pass->GetName() == passName; });
  if (iter == m_passes.end()) {
    LOG(WARNING) << FORMAT("can't find the pass: {}, won't execute it", passName);
    return;
  }

  auto& pass = *iter;

  if (!pass->IsEnable()) {
    LOG(WARNING) << FORMAT("can't run pass: {} because IsEnable function of this pass return false", passName);
    return;
  }

  pass->Execute(this);
  pass->Submit(waitSemaphore, signalSemaphore, fence);
}

}  // namespace Marbas
