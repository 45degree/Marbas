#include "DirectionLightShadowMapPass.hpp"

#include "AssetManager/ModelAsset.hpp"
#include "Common/MathCommon.hpp"
#include "Core/Common.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/RenderMeshComponent.hpp"
#include "Core/Scene/Component/ShadowComponent.hpp"

namespace Marbas {

DirectionShadowMapPass::DirectionShadowMapPass(const DirectionShadowMapPassCreateInfo& createInfo)
    : m_scene(createInfo.scene),
      m_shadowMapTextureHandler(createInfo.directionalShadowMap),
      m_rhiFactory(createInfo.rhiFactory) {
  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
}

void
DirectionShadowMapPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  constexpr int textureCount = DirectionShadowComponent::splitCount + 1;
  builder.WriteTexture(m_shadowMapTextureHandler, TextureAttachmentType::COLOR, 0, textureCount);

  builder.BeginPipeline();
  builder.AddShaderArgument(MeshGPUAsset::GetDescriptorSetArgument());
  builder.AddShaderArgument(m_argument);
  builder.AddShader("Shader/directionLightShadowMap.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/directionLightShadowMap.geom.spv", ShaderType::GEOMETRY_SHADER);
  builder.AddShader("Shader/directionLightShadowMap.frag.spv", ShaderType::FRAGMENT_SHADER);
  builder.SetVertexInputElementDesc(GetMeshVertexInfoLayout());
  builder.SetVertexInputElementView(GetMeshVertexViewInfo());
  builder.SetDepthTarget({
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::DEPTH_STENCIL | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
  });
  builder.EnableDepthTest(true);
  builder.EndPipeline();

  builder.SetFramebufferSize(m_width, m_height, DirectionShadowComponent::splitCount + 1);
}

void
DirectionShadowMapPass::Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandList) {
  // Create Shadow Alias
  auto& world = m_scene->GetWorld();
  auto view = world.view<DirectionShadowComponent>();
  auto shadowCount = view.size();

  int level = std::ceil(std::log(shadowCount) / std::log(4));
  std::vector<glm::mat3> aliasTrans(std::pow(4, level));
  aliasTrans[0] = glm::mat3(1.0);
  for (int i = 0; i < level; i++) {
    for (int j = std::pow(4, i) - 1; j >= 0; j--) {
      constexpr static std::array<float, 9> _t1 = {0.5, 0, 0, 0, 0.5, 0, 0, 0, 1};
      constexpr static std::array<float, 9> _t2 = {0.5, 0, 0, 0, 0.5, 0, 0.5, 0, 1};
      constexpr static std::array<float, 9> _t3 = {0.5, 0, 0, 0, 0.5, 0, 0, 0.5, 1};
      constexpr static std::array<float, 9> _t4 = {0.5, 0, 0, 0, 0.5, 0, 0.5, 0.5, 1};
      const static glm::mat3 t1 = glm::make_mat3(_t1.data());
      const static glm::mat3 t2 = glm::make_mat3(_t2.data());
      const static glm::mat3 t3 = glm::make_mat3(_t3.data());
      const static glm::mat3 t4 = glm::make_mat3(_t4.data());
      aliasTrans[4 * j + 3] = t4 * aliasTrans[j];
      aliasTrans[4 * j + 2] = t3 * aliasTrans[j];
      aliasTrans[4 * j + 1] = t2 * aliasTrans[j];
      aliasTrans[4 * j + 0] = t1 * aliasTrans[j];
    }
  }

  /**
   * render the scene
   */
  auto modelView = world.view<ModelSceneNode>();

  // load all model and calculate the sum of mesh
  auto modelManager = AssetManager<ModelAsset>::GetInstance();
  auto modelGPUManager = GPUAssetManager<ModelGPUAsset>::GetInstance();
  auto bufferContext = m_rhiFactory->GetBufferContext();

  /**
   * Record command
   */
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();

  std::array<ViewportInfo, 1> viewport;
  std::array<ScissorInfo, 1> scissor;

  commandList.Begin();
  commandList.BeginPipeline(pipeline, framebuffer, {{1, 1}});

  for (auto iter = view.begin(); iter != view.end(); iter++) {
    auto& shadowComp = view.get<DirectionShadowComponent>(*iter);
    auto& shadowInfo = shadowComp.GetShadowInfo();

    // calculate viewport
    auto index = std::distance(view.begin(), iter);
    auto viewPortEnd = aliasTrans[index] * glm::vec3(m_width, m_height, 1);
    auto viewPortStart = aliasTrans[index] * glm::vec3(0, 0, 1);
    auto length = viewPortEnd - viewPortStart;

    viewport[0].x = viewPortStart.x;
    viewport[0].y = viewPortStart.y;
    viewport[0].width = length.x;
    viewport[0].height = length.y;
    viewport[0].minDepth = 0;
    viewport[0].maxDepth = 1;

    scissor[0].x = viewPortStart.x;
    scissor[0].y = viewPortStart.y;
    scissor[0].width = length.x;
    scissor[0].height = length.y;

    commandList.SetViewports(viewport);
    commandList.SetScissors(scissor);

    for (auto&& [entity, modelSceneNode] : modelView.each()) {
      if (modelSceneNode.modelPath == "res://") {
        continue;
      }
      auto modelAsset = modelManager->Get(modelSceneNode.modelPath);
      auto modelGPUAsset = modelGPUManager->Get(modelAsset->GetUid());
      for (auto& meshGPUAsset : modelGPUAsset->m_meshGPUAsset) {
        auto& indexCount = meshGPUAsset->m_indexCount;

        // update transform matrix
        // bufferContext->UpdateBuffer(meshGPUAsset->m_transformBuffer, &model, sizeof(model), 0);

        commandList.BindDescriptorSet(pipeline, {meshGPUAsset->m_descriptorSet, shadowInfo.m_descriptorSet});
        commandList.BindVertexBuffer(meshGPUAsset->m_vertexBuffer);
        commandList.BindIndexBuffer(meshGPUAsset->m_indexBuffer);
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
