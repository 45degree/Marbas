#include "Core/Renderer/DirectionLightShadowMapRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Common.hpp"
#include "Core/Renderer/BlinnPhongRenderPass.hpp"
#include "Core/Renderer/DeferredRenderPass.hpp"
#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "Core/Scene/Component/ShadowComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "RHI/Interface/DescriptorSet.hpp"
#include "RHI/Interface/RenderPass.hpp"

namespace Marbas {

DirectionLightShadowMapCreateInfo::DirectionLightShadowMapCreateInfo()
    : DeferredRenderPassCreateInfo() {
  passName = String(DirectionLightShadowMapRenderPass::renderPassName);
  inputResource = {
      GeometryRenderPass::geometryTargetName,
  };
  outputResource = {
      String(DirectionLightShadowMapRenderPass::renderTarget),
  };
}

/**
 * shadow mapping render pass
 */

DirectionLightShadowMapRenderPass::DirectionLightShadowMapRenderPass(
    const DirectionLightShadowMapCreateInfo& createInfo)
    : DeferredRenderPass(createInfo) {
  m_lightUniformBuffer = m_rhiFactory->CreateUniformBuffer(sizeof(LightUniformBlock));
}

void
DirectionLightShadowMapRenderPass::OnInit() {}

void
DirectionLightShadowMapRenderPass::CreateRenderPass() {
  // depth render pass
  RenderPassCreateInfo depthRenderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
                  .format = TextureFormat::DEPTH,
                  .type = AttachmentType::Depth,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
          },
  };
  m_renderPass = m_rhiFactory->CreateRenderPass(depthRenderPassCreateInfo);
}

void
DirectionLightShadowMapRenderPass::CreateShader() {
  auto shaderContainer = m_resourceManager->GetContainer<ShaderResource>();

  // depth shader
  auto depthShaderRes = shaderContainer->CreateResource();
  depthShaderRes->SetShaderStage(ShaderType::VERTEX_SHADER,
                                 "Shader/directionLightShadowMap.vert.glsl");
  depthShaderRes->SetShaderStage(ShaderType::GEOMETRY_SHADER,
                                 "Shader/directionLightShadowMap.geom.glsl");
  depthShaderRes->SetShaderStage(ShaderType::FRAGMENT_SHADER,
                                 "Shader/directionLightShadowMap.frag.glsl");
  depthShaderRes->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(depthShaderRes);
}

void
DirectionLightShadowMapRenderPass::CreateFrameBuffer() {
  const auto& targetGBuffer = m_outputTarget[String(renderTarget)];
  auto shadowTexture = targetGBuffer->GetGBuffer(GBufferTexutreType::SHADOW_MAP);

  std::shared_ptr shadowTextureView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = shadowTexture,
      .m_format = shadowTexture->GetFormat(),
      .m_type = shadowTexture->GetTextureType(),
      .m_layerBase = 0,
      .m_layerCount = 1,
      .m_levelBase = 0,
      .m_levelCount = 1,
  });
  m_framebuffer = m_rhiFactory->CreateFrameBuffer(FrameBufferInfo{
      .width = 4096,
      .height = 4096,
      .renderPass = m_renderPass.get(),
      .attachments = {shadowTextureView},
  });
}

void
DirectionLightShadowMapRenderPass::CreateDescriptorSetLayout() {
  // depth binding layout
  m_descriptorSetLayout = {
      DescriptorSetLayoutBinding{
          // camera
          .isBuffer = true,
          .type = BufferDescriptorType::UNIFORM_BUFFER,
          .bindingPoint = 0,
      },
      DescriptorSetLayoutBinding{
          .isBuffer = true,
          .type = BufferDescriptorType::UNIFORM_BUFFER,
          .bindingPoint = 1,
      },
      DescriptorSetLayoutBinding{
          .isBuffer = true,
          .type = BufferDescriptorType::DYNAMIC_UNIFORM_BUFFER,
          .bindingPoint = 2,
      },
  };
}

void
DirectionLightShadowMapRenderPass::CreatePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();

  // depth pipeline
  auto depthShaderRes = shaderContainer->GetResource(m_shaderId);
  if (!depthShaderRes->IsLoad()) {
    depthShaderRes->LoadResource(m_rhiFactory, m_resourceManager.get());
  }

  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = 4096, .height = 4096});
  m_pipeline->SetVertexBufferLayout(GetMeshVertexInfoLayout(), VertexInputRate::VERTEX);
  m_pipeline->SetShader(depthShaderRes->GetShader());
  m_pipeline->SetPipelineLayout(GraphicsPipeLineLayout{
      .descriptorSetLayout = m_descriptorSetLayout,
  });
  m_pipeline->Create();
}

void
DirectionLightShadowMapRenderPass::SetUniformBuffer(const Scene* scene) {
  uint32_t index = 0;
  auto view = Entity::GetAllEntity<MeshComponent, ShadowComponent, HierarchyComponent>(
      const_cast<Scene*>(scene));
  for (auto entity : view) {
    auto& meshComponent = view.get<MeshComponent>(entity);
    const auto& hierarchyComponent = view.get<HierarchyComponent>(entity);

    if (meshComponent.m_impldata == nullptr) continue;
    auto size = ROUND_UP(sizeof(MeshComponent::UniformBufferBlockData), 32);
    auto offset = index * size;

    if (!meshComponent.m_model.expired()) {
      const auto model = meshComponent.m_model.lock();
      const auto& modelMatrix = hierarchyComponent.globalTransformMatrix;
      meshComponent.m_uniformBufferData.model = modelMatrix;
    }

    m_meshDynamicUniformBuffer->SetData(&meshComponent.m_uniformBufferData, size, offset);
    index++;
  }
}

void
DirectionLightShadowMapRenderPass::SetUniformBufferForLight(const Scene* scene, int lightIndex) {
  auto lightView = Entity::GetAllEntity<ParallelLightComponent>(const_cast<Scene*>(scene));
  auto& light = lightView.get<ParallelLightComponent>(lightView[lightIndex]).m_light;

  const auto viewMatrix = light.GetViewMatrix();
  const auto perspectiveMatrix = light.GetProjectionMatrix();
  m_lightUniformBlock.lightIndex = lightIndex;
  m_lightUniformBlock.view = viewMatrix;
  m_lightUniformBlock.projective = perspectiveMatrix;
  m_lightUniformBuffer->SetData(&m_lightUniformBlock, sizeof(LightUniformBlock), 0);
}

void
DirectionLightShadowMapRenderPass::RecordCommand(const Scene* scene) {
  m_commandBuffer->Clear();
  DLOG_ASSERT(m_renderPass != nullptr);

  auto view = Entity::GetAllEntity<MeshComponent, ShadowComponent>(const_cast<Scene*>(scene));
  auto entityCount = view.size_hint();

  // recreate dynamic uniform buffer
  auto bufferSize = entityCount * ROUND_UP(sizeof(MeshComponent::UniformBufferBlockData), 32);
  m_meshDynamicUniformBuffer = m_rhiFactory->CreateDynamicUniforBuffer(bufferSize);

  /**
   * set shadow depth command
   */

  m_commandBuffer->Clear();

  // record command
  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->BeginRenderPass(BeginRenderPassInfo{
      .renderPass = m_renderPass,
      .frameBuffer = m_framebuffer,
      .clearColor = {0, 0, 0, 1},
  });
  m_commandBuffer->BindPipeline(m_pipeline);

  uint32_t meshIndex = 0;
  for (auto entity : view) {
    auto& meshComponent = view.get<MeshComponent>(entity);
    auto& shadowComponent = view.get<ShadowComponent>(entity);
    const auto& meshImplData = meshComponent.m_impldata;
    auto& shadowImplData = shadowComponent.implData;

    shadowImplData->descriptorSet->BindDynamicBuffer(2, m_meshDynamicUniformBuffer);

    if (meshImplData->vertexBuffer != nullptr && meshImplData->indexBuffer != nullptr) {
      auto size = ROUND_UP(sizeof(MeshComponent::UniformBufferBlockData), 32);
      auto offset = size * meshIndex;

      m_commandBuffer->BindVertexBuffer(meshImplData->vertexBuffer);
      m_commandBuffer->BindIndexBuffer(meshImplData->indexBuffer);
      m_commandBuffer->BindDescriptorSet(BindDescriptorSetInfo{
          .descriptorSet = shadowImplData->descriptorSet,
          .layouts = m_descriptorSetLayout,
          .bufferPiece = {DynamicBufferPiece{
              .offset = static_cast<uint32_t>(offset),
              .size = static_cast<uint32_t>(size),
          }},
      });
      m_commandBuffer->DrawIndex(meshImplData->indexBuffer->GetIndexCount(), 0);

      meshIndex++;
    }
  }
  m_commandBuffer->EndRenderPass();
  m_commandBuffer->EndRecordCmd();
}

void
DirectionLightShadowMapRenderPass::CreateBufferForEveryEntity(const entt::entity& entity,
                                                              Scene* scene) {
  DLOG_ASSERT(Entity::HasComponent<ShadowComponent>(scene, entity));

  auto& shadowComponent = Entity::GetComponent<ShadowComponent>(scene, entity);
  if (shadowComponent.implData != nullptr) return;

  auto implData = std::make_shared<ShadowComponent_Impl>();

  implData->descriptorSet = m_rhiFactory->CreateDescriptorSet(m_descriptorSetLayout);
  implData->descriptorSet->BindBuffer(0, m_cameraUniformBuffer);
  implData->descriptorSet->BindBuffer(1, m_lightUniformBuffer);

  shadowComponent.implData = implData;
}

void
DirectionLightShadowMapRenderPass::Execute(const Scene* scene,
                                           const ResourceManager* resourceManager) {
  auto view = Entity::GetAllEntity<MeshComponent, ShadowComponent>(scene);
  auto lightView = Entity::GetAllEntity<ParallelLightComponent>(const_cast<Scene*>(scene));

  for (auto mesh : view) {
    CreateBufferForEveryEntity(mesh, const_cast<Scene*>(scene));
  }

  auto bufferSize = view.size_hint() * ROUND_UP(sizeof(MeshComponent::UniformBufferBlockData), 32);
  if (m_meshDynamicUniformBuffer == nullptr ||
      bufferSize != m_meshDynamicUniformBuffer->GetSize()) {
    RecordCommand(scene);
  }

  ClearDepth();
  SetUniformBuffer(scene);
  auto lightCount = lightView.size();
  for (int i = 0; i < lightCount; i++) {
    SetUniformBufferForLight(scene, i);
    m_commandBuffer->SubmitCommand();
  }
}

}  // namespace Marbas
