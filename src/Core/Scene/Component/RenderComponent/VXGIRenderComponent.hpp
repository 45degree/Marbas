#pragma once

#include "Common/MathCommon.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

/**
 * @class VXGIGlobalComponent
 * @brief 记录当前帧中需要使用到的VXGI的Radiance voxel
 *
 */
class VoxelRenderComponent;
struct VXGIGlobalComponent {
 public:
  static constexpr int maxProbeCount = 1;

 private:
  struct VoxelInfo {
    struct {
      glm::vec4 voxelSizeResolution;
      glm::vec3 pos = glm::vec3(0, 0, 0);
    } m_voxelInfo[maxProbeCount];
  } m_voxelsInfo;
  RHIFactory* m_rhiFactory;

 public:
  VXGIGlobalComponent(RHIFactory* rhiFactory);
  ~VXGIGlobalComponent();

  void
  BindVoxelProbe(const VoxelRenderComponent& component, uint32_t index = 0);

  void
  UpdateVoxelProbe(const VoxelRenderComponent& component, uint32_t index = 0);

 public:
  static const DescriptorSetArgument&
  GetDescriptorSetArgument();

 public:
  std::array<Image*, maxProbeCount> m_voxelRadiances;
  Buffer* m_infoBuffer;
  uintptr_t m_set;
};

struct VoxelRenderComponent {
  friend class VXGIGlobalComponent;

 private:
  struct VoxelInfo {
    glm::mat4 projX;
    glm::mat4 projY;
    glm::mat4 projZ;
    glm::vec4 voxelSizeResolution;
    glm::vec3 pos = glm::vec3(0, 0, 0);
  } m_voxelInfo;
  // ImageView* m_voxelStaticDiffuseView = nullptr;
  // ImageView* m_voxelStaticNormalView = nullptr;
  ImageView* m_voxelDiffuseView = nullptr;
  ImageView* m_voxelNormalView = nullptr;
  ImageView* m_voxelRadianceView = nullptr;
  ImageView* m_diffuseVoxelizationView = nullptr;
  ImageView* m_normalVoxelizationView = nullptr;
  uintptr_t m_sampler;
  uintptr_t m_radianceSampler;

 public:
  // Image* m_voxelStaticDiffuse = nullptr;
  // Image* m_voxelStaticNormal = nullptr;
  Image* m_voxelDiffuse = nullptr;
  Image* m_voxelNormal = nullptr;
  Image* m_voxelRadiance = nullptr;
  Buffer* m_giInfo = nullptr;
  // uintptr_t m_setForStaticVoxelization;
  uintptr_t m_setForVoxelization;  // 用于体素化场景的descriptor set
  uintptr_t m_setForLightInject;   // 用于光线注入的descriptor set
  uint32_t m_resolution;

 public:
  VoxelRenderComponent(RHIFactory* rhiFactory, const glm::vec3& size, uint32_t resolution = 256);
  ~VoxelRenderComponent();

 public:
  void
  UpdateVoxelInfo(const glm::vec3& size, const glm::vec3& pos = glm::vec3(0, 0, 0));

 public:
  static DescriptorSetArgument&
  GetVoxelizationDescriptorArgument();

  static DescriptorSetArgument&
  GetLightInjectDescriptorArgument();

 private:
  void
  BindVoxelizationSet();

  void
  BindLightInjectSet();

  void
  BindStaticVoxelizationSet();

 private:
  RHIFactory* m_rhiFactory;
};

}  // namespace Marbas
