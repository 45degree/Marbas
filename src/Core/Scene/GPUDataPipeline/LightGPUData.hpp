#pragma once

#include "Core/Scene/Component/Component.hpp"
#include "GPUDataPipeline.hpp"

namespace Marbas {

struct DirectionLightInfo {
  alignas(4) glm::vec3 direction = glm::vec3(0, 1, 0);
  alignas(16) glm::vec4 colorEnergy = glm::vec4(1, 1, 1, 1);
  alignas(4) glm::vec4 atlasViewport = glm::vec4(0, 0, 1, 1);
  alignas(4) glm::vec4 cascadePlaneDistances[DirectionShadowComponent::shadowMapArraySize] = {glm::vec4(1)};
  alignas(4) glm::mat4 lightMatrix[DirectionShadowComponent::shadowMapArraySize] = {glm::mat4(1)};
};

struct DirectionLightInfoList {
  alignas(4) int shadowLightCount = 0;
  alignas(4) int unshadowLightCount = 0;
  alignas(16) glm::ivec4 shadowLightIndexList[MAX_DIRECTION_LIGHT_COUNT] = {glm::ivec4(0)};
  alignas(16) glm::ivec4 unshadowLightIndexList[MAX_DIRECTION_LIGHT_COUNT] = {glm::ivec4(0)};
  alignas(16) DirectionLightInfo directionalLightInfo[MAX_DIRECTION_LIGHT_COUNT];
};

/**
 * @class LightGPUData
 * @brief the class used to hold all light info in the scene
 *
 */
class LightGPUData final : public GPUDataPipelineDataBase {
  static DescriptorSetArgument s_lightArgument;
  static uintptr_t s_lightSet;
  static Buffer* s_directionalLightBuffer;
  static DirectionLightInfoList s_directionalLightInfos;

 public:
  // TODO: implement
  static Buffer* s_pointLightBuffer;

 public:
  Task<>
  Load(DirectionLightComponent& light);

  Task<>
  Load(DirectionLightComponent& light, const DirectionShadowComponent& shadow);

  Task<>
  Update(const DirectionLightComponent& light);

  Task<>
  Update(const DirectionLightComponent& light, const DirectionShadowComponent& shadow);

  Task<>
  Uninstall(DirectionLightComponent& light);

 public:
  static const DescriptorSetArgument&
  GetDescriptorSetArgument() {
    return s_lightArgument;
  }

  static Buffer*
  GetDirectionLightBuffer() {
    return s_directionalLightBuffer;
  }

  static const DirectionLightInfoList&
  GetDirectionLightInfoList() {
    return s_directionalLightInfos;
  }

  static uintptr_t
  GetLightSet() {
    return s_lightSet;
  }

  /**
   * @brief Initialize all the static variable
   */
  static void
  Initialize(RHIFactory* rhiFactory);

  static void
  Destroy(RHIFactory* rhiFactory);

 private:
  void
  AssignLightIndex(DirectionLightComponent& light);

  void
  UpdateDirectionalLightGPUBuffer(uint32_t index);

  /**
   * @brief insert a unshadow directional light index to the unshadowLightIndexList of DirectionLightInfoList,
   *        if the index don't exist in the unshadowLightIndexList, unshadowLightCount++;
   *
   * @param index unshadow directional light index
   *
   * @see InsertShadowDirectionLight
   */
  static void
  InsertUnShadowDirectionalLight(uint32_t index) {
    int& count = s_directionalLightInfos.unshadowLightCount;
    InsertIndexToIndexList(glm::ivec4(index), std::span{s_directionalLightInfos.unshadowLightIndexList}, count);
  }

  /**
   * @brief insert a shadwo directional light index to the shadowLightIndexList of DirectionLightInfoList
   *
   * @param index shadow directional light index
   *
   * @see InsertDirectionalLight
   */
  static void
  InsertShadowDirectionLight(uint32_t index) {
    int& count = s_directionalLightInfos.shadowLightCount;
    InsertIndexToIndexList(glm::ivec4(index), std::span{s_directionalLightInfos.shadowLightIndexList}, count);
  }

  static void
  DeleteUnshadowDirectionLight(uint32_t index) {
    int& count = s_directionalLightInfos.unshadowLightCount;
    DeleteIndexFromIndexList(glm::ivec4(index), std::span{s_directionalLightInfos.unshadowLightIndexList}, count);
  }

  static void
  DeleteShadowDirectionLight(uint32_t index) {
    int& count = s_directionalLightInfos.shadowLightCount;
    DeleteIndexFromIndexList(glm::ivec4(index), std::span{s_directionalLightInfos.shadowLightIndexList}, count);
  }

  template <typename T, std::size_t N>
  static void
  InsertIndexToIndexList(T index, std::span<T, N> indexList, int& count) {
    auto end = count > indexList.size() ? indexList.end() : indexList.begin() + count;
    auto iter = std::find(indexList.begin(), end, index);
    if (iter != end) {
      return;
    }
    indexList[count++] = index;
    return;
  }

  template <typename T, std::size_t N>
  static void
  DeleteIndexFromIndexList(T index, std::span<T, N> indexList, int& count) {
    auto end = count > indexList.size() ? indexList.end() : indexList.begin() + count;
    auto iter = std::find(indexList.begin(), end, index);
    if (iter == end) return;

    std::memcpy(&(*iter), &(*(iter + 1)), sizeof(T) * std::distance(iter + 1, end));
    count--;
  }

  template <typename T, std::size_t N>
  static bool
  IsInList(T index, std::span<T, N> indexList, int count) {
    auto end = count > indexList.size() ? indexList.end() : indexList.begin() + count;
    auto iter = std::find(indexList.begin(), end, index);
    return iter != end;
  }
};

using LightGPUDataManager = Singleton<GPUDataPipelineBase<entt::entity, LightGPUData>>;

}  // namespace Marbas
