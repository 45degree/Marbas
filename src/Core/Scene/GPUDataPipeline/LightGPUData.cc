#pragma once

#include "LightGPUData.hpp"

namespace Marbas {

Buffer* LightGPUData::s_directionalLightBuffer = nullptr;
Buffer* LightGPUData::s_pointLightBuffer = nullptr;
uintptr_t LightGPUData::s_lightSet = 0;
DescriptorSetArgument LightGPUData::s_lightArgument = DescriptorSetArgument({
    {0, DescriptorType::UNIFORM_BUFFER},
});
DirectionLightInfoList LightGPUData::s_directionalLightInfos;

static void
SetDirectionLightInfo(DirectionLightInfo& info, const DirectionLightComponent& light) {
  info.color = light.m_light.GetColor();

  // TODO: change light energy
  info.energy.x = 30;
  info.direction = light.m_light.GetDirection();
}

static void
SetDirectionLightInfo(DirectionLightInfo& info, const DirectionLightComponent& light,
                      const DirectionShadowComponent& shadow) {
  SetDirectionLightInfo(info, light);

  for (int i = 0; i < shadow.m_lightSpaceMatrices.size(); i++) {
    info.lightMatrix[i] = shadow.m_lightSpaceMatrices.at(i);
  }

  info.atlasViewport = shadow.m_viewport;
  for (int i = 0; i < shadow.m_cascadePlane.size(); i++) {
    info.cascadePlaneDistances[i].x = shadow.m_cascadePlane[i];
  }
}

void
LightGPUData::Initialize(RHIFactory* rhiFactory) {
  auto bufCtx = rhiFactory->GetBufferContext();
  auto pipelineCtx = rhiFactory->GetPipelineContext();
  auto bufferSize = sizeof(DirectionLightInfoList);
  auto bufferData = &s_directionalLightInfos;
  s_directionalLightBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, bufferData, bufferSize, false);

  s_lightSet = pipelineCtx->CreateDescriptorSet(s_lightArgument);
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = s_lightSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = s_directionalLightBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

void
LightGPUData::Destroy(RHIFactory* rhiFactory) {}

void
LightGPUData::AssignLightIndex(DirectionLightComponent& light) {
  auto& shadowList = s_directionalLightInfos.shadowLightIndexList;
  auto& unshadowList = s_directionalLightInfos.unshadowLightIndexList;
  auto shadowCount = s_directionalLightInfos.shadowLightCount;
  auto unshadowCount = s_directionalLightInfos.unshadowLightCount;
  for (uint32_t i = 0; i < MAX_DIRECTION_LIGHT_COUNT; i++) {
    bool isInShadow = IsInList(glm::ivec4(i), std::span{shadowList}, shadowCount);
    bool isInUnShadow = IsInList(glm::ivec4(i), std::span{unshadowList}, unshadowCount);
    if (!isInShadow && !isInUnShadow) {
      light.lightIndex = i;
      break;
    }
  }

  return;
}

void
LightGPUData::UpdateDirectionalLightGPUBuffer(uint32_t index) {
  auto bufferContext = m_rhiFactory->GetBufferContext();

  // update index
  size_t size = offsetof(DirectionLightInfoList, directionalLightInfo[0]);
  bufferContext->UpdateBuffer(s_directionalLightBuffer, &s_directionalLightInfos, size, 0);

  // update details info
  auto& lightInfo = s_directionalLightInfos.directionalLightInfo[index];
  uintptr_t offset = offsetof(DirectionLightInfoList, directionalLightInfo[index]);
  size = sizeof(DirectionLightInfo);
  bufferContext->UpdateBuffer(s_directionalLightBuffer, &lightInfo, size, offset);
}

Task<>
LightGPUData::Load(DirectionLightComponent& light) {
  AssignLightIndex(light);

  if (!light.lightIndex.has_value()) co_return;

  auto index = light.lightIndex.value();
  InsertUnShadowDirectionalLight(index);
  SetDirectionLightInfo(s_directionalLightInfos.directionalLightInfo[index], light);
  UpdateDirectionalLightGPUBuffer(index);
  co_return;
}

Task<>
LightGPUData::Load(DirectionLightComponent& light, const DirectionShadowComponent& shadow) {
  AssignLightIndex(light);

  if (!light.lightIndex.has_value()) co_return;

  auto index = light.lightIndex.value();
  InsertShadowDirectionLight(index);
  SetDirectionLightInfo(s_directionalLightInfos.directionalLightInfo[index], light, shadow);
  UpdateDirectionalLightGPUBuffer(index);
  co_return;
}

Task<>
LightGPUData::Update(const DirectionLightComponent& light) {
  if (!light.lightIndex.has_value()) co_return;
  auto index = light.lightIndex.value();

  // updatre light index buffer
  InsertUnShadowDirectionalLight(index);
  DeleteShadowDirectionLight(index);

  // update light info
  auto& lightInfo = s_directionalLightInfos.directionalLightInfo[index];
  SetDirectionLightInfo(lightInfo, light);
  UpdateDirectionalLightGPUBuffer(index);
  co_return;
}

Task<>
LightGPUData::Update(const DirectionLightComponent& light, const DirectionShadowComponent& shadow) {
  if (!light.lightIndex.has_value()) co_return;
  auto index = light.lightIndex.value();

  // updatre light index buffer
  InsertShadowDirectionLight(index);
  DeleteUnshadowDirectionLight(index);

  // update light info
  auto& lightInfo = s_directionalLightInfos.directionalLightInfo[index];
  SetDirectionLightInfo(lightInfo, light, shadow);
  UpdateDirectionalLightGPUBuffer(index);
  co_return;
}

Task<>
LightGPUData::Uninstall(DirectionLightComponent& light) {
  if (!light.lightIndex.has_value()) co_return;

  auto index = light.lightIndex.value();
  DeleteShadowDirectionLight(index);
  DeleteUnshadowDirectionLight(index);

  light.lightIndex.reset();
  co_return;
}

}  // namespace Marbas
