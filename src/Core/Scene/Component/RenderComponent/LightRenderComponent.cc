#pragma once

#include "LightRenderComponent.hpp"

#include <glog/logging.h>

#include <mutex>

#include "Core/Scene/Component/SerializeComponent/LightComponent.hpp"

namespace Marbas {

LightRenderComponent::LightRenderComponent(RHIFactory* rhiFactory) : m_rhiFactory(rhiFactory) {
  auto bufCtx = rhiFactory->GetBufferContext();
  auto pipelineCtx = rhiFactory->GetPipelineContext();
  auto bufferSize = sizeof(DirectionLightInfoList);
  auto bufferData = &m_directionalLightInfos;
  m_directionalLightBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, bufferData, bufferSize, false);

  m_lightSet = pipelineCtx->CreateDescriptorSet(GetDescriptorSetArgument());
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_lightSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_directionalLightBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

LightRenderComponent::~LightRenderComponent() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  pipelineCtx->DestroyDescriptorSet(m_lightSet);
  bufCtx->DestroyBuffer(m_directionalLightBuffer);
}

const DescriptorSetArgument&
LightRenderComponent::GetDescriptorSetArgument() {
  static DescriptorSetArgument lightArgument;
  static std::once_flag onceFlags;

  std::call_once(onceFlags, [&]() {
    lightArgument.Clear();
    lightArgument.Bind(0, DescriptorType::UNIFORM_BUFFER);

    // TODO: support for point and spot light
    // m_lightArgument.Bind(1, DescriptorType::UNIFORM_BUFFER);
    // m_lightArgument.Bind(2, DescriptorType::UNIFORM_BUFFER);
    return;
  });

  return lightArgument;
}

void
LightRenderComponent::AddLight(DirectionLightComponent& light) {
  if (m_directionalLightInfos.lightCount >= MAX_DIRECTION_LIGHT_COUNT) {
    LOG(WARNING) << FORMAT(
        "The number of directional lights exceeds the upper limit:{},"
        " and directional lights cannot be added",
        MAX_DIRECTION_LIGHT_COUNT);
    return;
  }
  light.lightIndex = m_directionalLightInfos.lightCount++;
}

void
LightRenderComponent::RemoveLight(DirectionLightComponent& light) {
  if (!CheckLight(light)) return;

  // TODO: implement
  throw std::logic_error("not implement");
}

void
LightRenderComponent::UpdateLight(const DirectionLightComponent& light) {
  if (!CheckLight(light)) return;

  auto& lightInfo = m_directionalLightInfos.directionalLightInfo[*light.lightIndex];
  lightInfo.directionShadow = glm::vec4(light.m_direction, 0);
  lightInfo.colorEnergy = glm::vec4(light.m_color, light.m_energy);

  UpdateGPUBuffer(light);
}

void
LightRenderComponent::UpdateLight(const DirectionLightComponent& light, const DirectionShadowComponent& shadow) {
  if (!CheckLight(light)) return;

  auto& lightInfo = m_directionalLightInfos.directionalLightInfo[*light.lightIndex];
  lightInfo.directionShadow = glm::vec4(light.m_direction, 1);
  lightInfo.colorEnergy = glm::vec4(light.m_color, light.m_energy);

  for (int i = 0; i < shadow.m_lightSpaceMatrices.size(); i++) {
    lightInfo.lightMatrix[i] = shadow.m_lightSpaceMatrices.at(i);
  }

  lightInfo.atlasViewport = shadow.m_viewport;
  for (int i = 0; i < shadow.m_cascadePlane.size(); i++) {
    lightInfo.cascadePlaneDistances[i].x = shadow.m_cascadePlane[i];
  }

  UpdateGPUBuffer(light);
}

void
LightRenderComponent::UpdateGPUBuffer(const DirectionLightComponent& light) {
  auto bufferContext = m_rhiFactory->GetBufferContext();

  // update index
  size_t size = offsetof(DirectionLightInfoList, directionalLightInfo[0]);
  bufferContext->UpdateBuffer(m_directionalLightBuffer, &m_directionalLightInfos, size, 0);

  // update details info
  auto index = *light.lightIndex;
  auto& lightInfo = m_directionalLightInfos.directionalLightInfo[index];
  uintptr_t offset = offsetof(DirectionLightInfoList, directionalLightInfo[0]) + index * size;
  size = sizeof(DirectionLightInfo);
  bufferContext->UpdateBuffer(m_directionalLightBuffer, &lightInfo, size, offset);
}

bool
LightRenderComponent::CheckLight(const DirectionLightComponent& light) {
  if (!light.lightIndex) {
    LOG(WARNING) << "light index is empty";
    return false;
  }

  if (*light.lightIndex >= MAX_DIRECTION_LIGHT_COUNT) {
    LOG(WARNING) << FORMAT(
        "The number of directional lights exceeds the upper limit:{},"
        " and directional lights cannot be update",
        MAX_DIRECTION_LIGHT_COUNT);
    return false;
  }
  return true;
}

}  // namespace Marbas
