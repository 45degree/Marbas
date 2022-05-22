#pragma once

#include "Common/Common.hpp"
#include "Core/Renderer/GBuffer.hpp"

namespace Marbas {

struct RenderTargetNodeCreateInfo {
  String targetName;
  Vector<GBufferTexutreType> buffersType;
  RHIFactory* rhiFactory;
  uint32_t width;
  uint32_t height;
};

class RenderTargetNode final {
 public:
  explicit RenderTargetNode(const RenderTargetNodeCreateInfo& createInfo);

  std::shared_ptr<GBuffer>
  GetGBuffer() const {
    return m_gbuffer;
  }

  const String&
  GetTargetName() const noexcept {
    return m_targetName;
  }

 protected:
  String m_targetName;
  Vector<GBufferTexutreType> m_gbufferType;
  RHIFactory* m_rhiFactory = nullptr;

  std::shared_ptr<GBuffer> m_gbuffer;
  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas
