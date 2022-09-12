#pragma once

#include "Common/Common.hpp"
#include "Core/Renderer/GBuffer.hpp"

namespace Marbas {

struct RenderTargetNodeCreateInfo {
  String targetName;
  Vector<GBufferType> buffersType;
  RHIFactory* rhiFactory = nullptr;
  uint32_t width = 1920;
  uint32_t height = 1080;
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
  Vector<GBufferType> m_gbufferType;
  RHIFactory* m_rhiFactory = nullptr;

  std::shared_ptr<GBuffer> m_gbuffer;
  uint32_t m_width;
  uint32_t m_height;
};

}  // namespace Marbas
