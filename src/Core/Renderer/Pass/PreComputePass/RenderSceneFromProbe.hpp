#pragma once

#include <entt/entt.hpp>

#include "AssetManager/Uid.hpp"
#include "Common/MathCommon.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"

namespace Marbas {

class RenderSceneFromProbe {
  struct DescriptorComponent {
    DescriptorPool* pool;
    DescriptorSet* set;

   private:
    RHIFactory* rhiFactory;

   public:
    DescriptorComponent(RHIFactory* rhiFactory, DescriptorSetLayout* descriptorSetLayout);
    ~DescriptorComponent();
  };

 public:
  RenderSceneFromProbe(RHIFactory* rhiFactory);
  virtual ~RenderSceneFromProbe() = default;

 public:
  void
  SetProbe(glm::vec3 pos) {
    m_probePos = pos;
  }

  struct RenderInfo {
    Scene* scene;
    uint32_t width;
    uint32_t height;
    ImageView* posCubemap;
    ImageView* normalCubemap;
    ImageView* depthCubemap;
  };

  void
  Render(const RenderInfo& renderInfo);

 private:
  void
  CreatePipeline();

  void
  CreateShader();

  void
  RecordCommand(Scene* scene, FrameBuffer* frameBuffer);

  void
  LoadRenderData(entt::registry& world, entt::entity entity);

 private:
  struct CameraInfo {
    glm::mat4 project;
    glm::mat4 view[6];
  } m_cameraInfo;

  Uid m_shaderId;
  glm::vec3 m_probePos;
  RHIFactory* m_rhiFactory;
  Pipeline* m_pipeline;
  DescriptorSetLayout* m_descriptorSetLayout;
  Buffer* m_cameraBuffer;

  CommandPool* m_commandPool;
  CommandBuffer* m_commandBuffer;

  Fence* m_fence;
};

}  // namespace Marbas
