#include "DirectionLightShadowMapPass.hpp"

#include "AssetManager/ModelAsset.hpp"
#include "Common/MathCommon.hpp"
#include "Core/Common.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/RenderMeshComponent.hpp"
#include "Core/Scene/Component/ShadowComponent.hpp"
#include "Core/Scene/GPUDataPipeline/LightGPUData.hpp"
#include "Core/Scene/GPUDataPipeline/ModelGPUData.hpp"

namespace Marbas {

DirectionShadowMapPass::DirectionShadowMapPass(const DirectionShadowMapPassCreateInfo& createInfo)
    : m_scene(createInfo.scene),
      m_shadowMapTextureHandler(createInfo.directionalShadowMap),
      m_rhiFactory(createInfo.rhiFactory) {}

void
DirectionShadowMapPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  constexpr int textureCount = DirectionShadowComponent::splitCount + 1;
  builder.WriteTexture(m_shadowMapTextureHandler, TextureAttachmentType::COLOR, 0, textureCount);

  builder.BeginPipeline();
  builder.AddShaderArgument(MeshGPUData::GetDescriptorSetArgument());
  builder.AddShaderArgument(LightGPUData::GetDescriptorSetArgument());
  builder.SetPushConstantSize(sizeof(int));
  builder.AddShader("Shader/directionLightShadowMap.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/directionLightShadowMap.geom.spv", ShaderType::GEOMETRY_SHADER);
  builder.AddShader("Shader/directionLightShadowMap.frag.spv", ShaderType::FRAGMENT_SHADER);
  builder.SetVertexInputElementDesc({
      {0, ElementType::R32G32B32_SFLOAT, 0, offsetof(Vertex, posX), 0},
  });
  builder.SetVertexInputElementView(GetMeshVertexViewInfo());
  builder.SetDepthTarget({
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::DEPTH_STENCIL | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
  });
  builder.EnableDepthTest(true);
  builder.EndPipeline();

  constexpr uint32_t width = DirectionShadowComponent::MAX_SHADOWMAP_SIZE;
  constexpr uint32_t height = DirectionShadowComponent::MAX_SHADOWMAP_SIZE;
  builder.SetFramebufferSize(width, height, DirectionShadowComponent::splitCount + 1);
}

void
DirectionShadowMapPass::Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandList) {
  // Create Shadow Alias
  auto& world = m_scene->GetWorld();
  auto view = world.view<DirectionLightComponent, DirectionShadowComponent>();
  auto shadowCount = view.size_hint();

  /**
   * render the scene
   */
  auto modelView = world.view<ModelSceneNode, RenderComponent>();

  // load all model and calculate the sum of mesh
  auto modelManager = AssetManager<ModelAsset>::GetInstance();
  auto modelGPUManager = ModelGPUDataManager::GetInstance();
  auto bufferContext = m_rhiFactory->GetBufferContext();

  /**
   * Record command
   */
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();

  commandList.Begin();
  commandList.BeginPipeline(pipeline, framebuffer, {{1, 1}});

  for (const auto& [entity, light, shadow] : view.each()) {
    if (!light.lightIndex.has_value()) continue;

    m_currentLightIndex = light.lightIndex.value();
    commandList.PushConstant(pipeline, &m_currentLightIndex, sizeof(int), 0);

    ViewportInfo viewportInfo;
    ScissorInfo scissorInfo;

    glm::vec4 viewport = shadow.m_viewport * (float)DirectionShadowComponent::MAX_SHADOWMAP_SIZE;
    viewportInfo.x = viewport.x;
    viewportInfo.y = viewport.y;
    viewportInfo.width = viewport.z;
    viewportInfo.height = viewport.w;
    viewportInfo.minDepth = 0;
    viewportInfo.maxDepth = 1;
    scissorInfo.x = viewport.x;
    scissorInfo.y = viewport.y;
    scissorInfo.width = viewport.z;
    scissorInfo.height = viewport.w;

    commandList.SetViewports({&viewportInfo, 1});
    commandList.SetScissors({&scissorInfo, 1});

    for (auto&& [entity, modelSceneNode] : modelView.each()) {
      if (modelSceneNode.modelPath == "res://") continue;

      auto modelAsset = modelManager->Get(modelSceneNode.modelPath);
      auto modelGPUAsset = modelGPUManager->TryGet(*modelAsset);
      auto meshCount = modelGPUAsset->MeshCount();
      for (size_t i = 0; i < meshCount; i++) {
        auto meshGPUData = modelGPUAsset->GetMeshGPU(i);
        auto& indexCount = meshGPUData->m_indexCount;

        std::vector<uintptr_t> sets = {meshGPUData->m_descriptorSet, LightGPUData::GetLightSet()};
        commandList.BindDescriptorSet(pipeline, sets);
        commandList.BindVertexBuffer(meshGPUData->m_vertexBuffer);
        commandList.BindIndexBuffer(meshGPUData->m_indexBuffer);
        commandList.DrawIndexed(indexCount, 1, 0, 0, 0);
      }
    }
  }

  commandList.EndPipeline(pipeline);
  commandList.End();
}

bool
DirectionShadowMapPass::IsEnable() {
  auto& world = m_scene->GetWorld();
  auto view = world.view<DirectionShadowComponent>();
  auto shadowCount = view.size();
  return shadowCount != 0;
}

}  // namespace Marbas
