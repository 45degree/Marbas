#pragma once

#include <entt/entt.hpp>

#include "Common/Camera.hpp"
#include "Common/Light.hpp"
#include "Common/MathCommon.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphResource.hpp"
#include "Core/Tool/Quadtree.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

/**
 * all direction shadow map store in a 2d texture array,
 * this component only store the subresource of the image
 */
struct DirectionShadowComponent {
  constexpr static int splitCount = 3;
  std::array<float, splitCount> m_split = {0.1, 0.2, 0.5};
  std::array<glm::mat4, splitCount + 1> m_lightSpaceMatrices;

  struct ShadowGPUInfo {
    Buffer* m_lightMatricesBuffer = nullptr;
    uintptr_t m_descriptorSet = 0;
    DescriptorSetArgument m_argument;
  };

 private:
  ShadowGPUInfo m_shadowGPUInfo;

 public:
  DirectionShadowComponent();

  const ShadowGPUInfo&
  GetShadowInfo() {
    return m_shadowGPUInfo;
  }

  void
  UpdateShadowGPUInfo(RHIFactory* rhiFactory, const glm::vec3& lightDir, const Camera& camera);
};

}  // namespace Marbas
