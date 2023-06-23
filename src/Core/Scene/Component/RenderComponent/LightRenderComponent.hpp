#pragma once

#include "Common/MathCommon.hpp"
#include "Core/Scene/Component/SerializeComponent/LightComponent.hpp"
#include "Core/Scene/Component/SerializeComponent/ShadowComponent.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

/**
 * @class LightRenderComponent
 * @brief global component to save all the light render information
 *
 */
struct LightRenderComponent {
  struct DirectionLightInfo {
    alignas(4) glm::vec4 directionShadow = glm::vec4(0, 1, 0, 1);
    alignas(16) glm::vec4 colorEnergy = glm::vec4(1, 1, 1, 1);
    alignas(4) glm::vec4 atlasViewport = glm::vec4(0, 0, 1, 1);
    alignas(4) glm::vec4 cascadePlaneDistances[DirectionShadowComponent::shadowMapArraySize] = {glm::vec4(1)};
    alignas(4) glm::mat4 lightMatrix[DirectionShadowComponent::shadowMapArraySize] = {glm::mat4(1)};
  };

  struct DirectionLightInfoList {
    alignas(4) int lightCount = 0;
    alignas(16) DirectionLightInfo directionalLightInfo[MAX_DIRECTION_LIGHT_COUNT];
  };

  uintptr_t m_lightSet;
  Buffer* m_directionalLightBuffer;

 public:
  LightRenderComponent(RHIFactory* rhiFactory);
  ~LightRenderComponent();

  static const DescriptorSetArgument&
  GetDescriptorSetArgument();

  void
  AddLight(DirectionLightComponent& light);

  void
  RemoveLight(DirectionLightComponent& light);

  void
  UpdateLight(const DirectionLightComponent& light);

  void
  UpdateLight(const DirectionLightComponent& light, const DirectionShadowComponent& shadow);

 private:
  void
  UpdateGPUBuffer(const DirectionLightComponent& light);

  bool
  CheckLight(const DirectionLightComponent& light);

 private:
  RHIFactory* m_rhiFactory = nullptr;
  DirectionLightInfoList m_directionalLightInfos;
};

}  // namespace Marbas
