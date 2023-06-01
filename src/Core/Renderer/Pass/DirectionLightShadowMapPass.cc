#include "DirectionLightShadowMapPass.hpp"

#include "AssetManager/ModelAsset.hpp"
#include "Common/MathCommon.hpp"
#include "Core/Common.hpp"
#include "Core/Scene/Component/Component.hpp"
#include "Core/Scene/GPUDataPipeline/LightGPUData.hpp"
#include "Core/Scene/GPUDataPipeline/MeshGPUData.hpp"

namespace Marbas {

DirectionShadowMapPass::DirectionShadowMapPass(const DirectionShadowMapPassCreateInfo& createInfo)
    : m_shadowMapTextureHandler(createInfo.directionalShadowMap), m_rhiFactory(createInfo.rhiFactory) {}

void
DirectionShadowMapPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  constexpr int textureCount = DirectionShadowComponent::splitCount + 1;
  builder.WriteTexture(m_shadowMapTextureHandler, TextureAttachmentType::COLOR, 0, textureCount);

  builder.BeginPipeline();
  // builder.AddShaderArgument(MeshGPUData::GetDescriptorSetArgument());
  builder.AddShaderArgument(LightGPUData::GetDescriptorSetArgument());
  builder.SetPushConstantSize(sizeof(Constant));
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
DirectionShadowMapPass::Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList) {
  // Create Shadow Alias
  auto* scene = registry.GetCurrentActiveScene();
  auto& world = scene->GetWorld();
  auto view = world.view<DirectionLightComponent, DirectionShadowComponent>();
  auto shadowCount = view.size_hint();

  /**
   * render the scene
   */
  auto modelView = world.view<ModelSceneNode, RenderableTag>();

  // load all model and calculate the sum of mesh
  auto modelManager = AssetManager<ModelAsset>::GetInstance();
  auto meshGPUManager = MeshGPUDataManager::GetInstance();
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

    m_constant.lightIndex = light.lightIndex.value();

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
      glm::mat4 model = glm::mat4(1.0);
      if (world.any_of<TransformComp>(entity)) {
        const auto& transformComp = world.get<TransformComp>(entity);
        model = transformComp.GetGlobalTransform();
      }
      m_constant.model = model;

      commandList.PushConstant(pipeline, &m_constant, sizeof(Constant), 0);

      for (auto mesh : modelSceneNode.m_meshEntities) {
        if (!world.any_of<RenderableMeshTag>(mesh)) continue;

        auto data = meshGPUManager->Get(mesh);
        if (data == nullptr) continue;

        auto& indexCount = data->m_indexCount;

        std::vector<uintptr_t> sets = {LightGPUData::GetLightSet()};
        commandList.BindDescriptorSet(pipeline, sets);
        commandList.BindVertexBuffer(data->m_vertexBuffer);
        commandList.BindIndexBuffer(data->m_indexBuffer);
        commandList.DrawIndexed(indexCount, 1, 0, 0, 0);
      }
    }
  }

  commandList.EndPipeline(pipeline);
  commandList.End();
}

bool
DirectionShadowMapPass::IsEnable(RenderGraphGraphicsRegistry& registry) {
  auto scene = registry.GetCurrentActiveScene();
  if (scene == nullptr) return false;

  auto& world = scene->GetWorld();
  auto view = world.view<DirectionShadowComponent>();
  auto shadowCount = view.size();
  return shadowCount != 0;
}

}  // namespace Marbas
