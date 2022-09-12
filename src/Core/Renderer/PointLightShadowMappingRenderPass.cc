#include "Core/Renderer/PointLightShadowMappingRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Renderer/ShadowMappingRenderPass.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/ShadowComponent.hpp"

namespace Marbas {

static Vector<ElementLayout>
GetMeshVertexInfoLayout() {
  Vector<ElementLayout> layouts{
      ElementLayout{0, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
      ElementLayout{1, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
      ElementLayout{2, ElementType::FLOAT, sizeof(float), 2, false, 0, 0},
  };

  ElementLayout::CalculateLayout(layouts);

  return layouts;
};

PointLightShadowMappingRenderPassCreateInfo::PointLightShadowMappingRenderPassCreateInfo() {
  passName = PointLightShadowMappingRenderPass::renderPassName;
  inputResource = {
      GeometryRenderPass::geometryTargetName,
  };
  outputResource = {
      String(PointLightShadowMappingRenderPass::targetName),
  };
}

/**
 * point light shadow mapping render pass
 */

PointLightShadowMappingRenderPass::PointLightShadowMappingRenderPass(const CreateInfo &createInfo)
    : DeferredRenderPass(createInfo) {
  m_lightInfoUniformBuffer = m_rhiFactory->CreateUniformBuffer(sizeof(LightInfo));
}

void
PointLightShadowMappingRenderPass::CreateRenderPass() {
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
PointLightShadowMappingRenderPass::CreateShader() {
  auto shaderContainer = m_resourceManager->GetContainer<ShaderResource>();

  // depth shader
  auto depthShaderRes = shaderContainer->CreateResource();
  depthShaderRes->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/pointLightDepth.vert.glsl");
  depthShaderRes->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/pointLightDepth.frag.glsl");
  depthShaderRes->SetShaderStage(ShaderType::GEOMETRY_SHADER, "Shader/pointLightDepth.geom.glsl");
  depthShaderRes->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(depthShaderRes);
}

void
PointLightShadowMappingRenderPass::CreateDescriptorSetLayout() {
  // depth decriptor set layout
  m_descriptorSetLayout = {
      DescriptorSetLayoutBinding{
          // camera
          .isBuffer = true,
          .type = BufferDescriptorType::UNIFORM_BUFFER,
          .bindingPoint = 0,
      },
      DescriptorSetLayoutBinding{
          // light info
          .isBuffer = true,
          .type = BufferDescriptorType::UNIFORM_BUFFER,
          .bindingPoint = 1,
      },
      DescriptorSetLayoutBinding{
          // model matrices
          .isBuffer = true,
          .type = BufferDescriptorType::DYNAMIC_UNIFORM_BUFFER,
          .bindingPoint = 2,
      },
  };
}

void
PointLightShadowMappingRenderPass::OnInit() {}

void
PointLightShadowMappingRenderPass::CreateFrameBuffer() {
  const auto &targetGBuffer = m_outputTarget[String(targetName)]->GetGBuffer();
  auto shadowTexture = targetGBuffer->GetTexture(GBufferTexutreType::SHADOW_MAP_CUBE);

  // depth framebuffer
  std::shared_ptr shadowTextureView = m_rhiFactory->CreateImageView();
  shadowTextureView->SetTexture(shadowTexture);
  m_framebuffer = m_rhiFactory->CreateFrameBuffer(FrameBufferInfo{
      .width = 4096,
      .height = 4096,
      .renderPass = m_renderPass.get(),
      .attachments = {shadowTextureView},
  });
}

void
PointLightShadowMappingRenderPass::CreateBufferForEveryEntity(const entt::entity &entity,
                                                              Scene *scene) {
  DLOG_ASSERT(Entity::HasComponent<ShadowComponent>(scene, entity));

  auto &shadowComponent = Entity::GetComponent<ShadowComponent>(scene, entity);
  if (shadowComponent.pointImplData != nullptr) return;

  auto implData = std::make_shared<PointShadowComponent_Impl>();
  implData->descriptorSet = m_rhiFactory->CreateDescriptorSet(m_descriptorSetLayout);
  implData->descriptorSet->BindBuffer(0, m_cameraUniformBuffer);
  implData->descriptorSet->BindBuffer(1, m_lightInfoUniformBuffer);

  shadowComponent.pointImplData = implData;
  m_needToRecordComand = true;
}

void
PointLightShadowMappingRenderPass::SetUniformBuffer(const Scene *scene) {
  uint32_t index = 0;
  auto view = Entity::GetAllEntity<MeshComponent, ShadowComponent>(const_cast<Scene *>(scene));
  for (auto entity : view) {
    auto &meshComponent = view.get<MeshComponent>(entity);

    if (meshComponent.m_impldata == nullptr) continue;
    auto offset = index * sizeof(MeshComponent::UniformBufferBlockData);
    auto size = sizeof(MeshComponent::UniformBufferBlockData);

    if (!meshComponent.m_model.expired()) {
      const auto model = meshComponent.m_model.lock();
      const auto &modelMatrix = model->GetModelMatrix();
      meshComponent.m_uniformBufferData.model = modelMatrix;
    }

    m_meshDynamicUniformBuffer->SetData(&meshComponent.m_uniformBufferData, size, offset);
    index++;
  }
}

void
PointLightShadowMappingRenderPass::SetUniformBufferForLight(const Scene *scene, int lightIndex) {
  auto lightView = Entity::GetAllEntity<PointLightComponent>(const_cast<Scene *>(scene));
  auto &light = lightView.get<PointLightComponent>(lightView[lightIndex]).m_light;

  m_lightInfos.lightIndex = lightIndex;
  m_lightInfos.pos = light.GetPos();
  m_lightInfos.projectMatrix = light.GetProjectionMatrix();
  m_lightInfos.farPlane = light.GetFarPlane();
  for (int i = 0; i < 6; i++) {
    m_lightInfos.matrixes[i] = light.GetViewMatrix(i);
  }

  m_lightInfoUniformBuffer->SetData(&m_lightInfos, sizeof(LightInfo), 0);
}

void
PointLightShadowMappingRenderPass::RecordCommand(const Scene *scene) {
  m_commandBuffer->Clear();
  // check framebuffer and renderpass
  DLOG_ASSERT(m_renderPass != nullptr);

  auto view = Entity::GetAllEntity<MeshComponent, ShadowComponent>(const_cast<Scene *>(scene));
  auto entityCount = view.size_hint();

  // recreate dynamic uniform buffer
  auto bufferSize = entityCount * sizeof(MeshComponent::UniformBufferBlockData);
  m_meshDynamicUniformBuffer = m_rhiFactory->CreateDynamicUniforBuffer(bufferSize);

  /**
   * set shadow depth command
   */

  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->BeginRenderPass(BeginRenderPassInfo{
      .renderPass = m_renderPass,
      .frameBuffer = m_framebuffer,
      .clearColor = {0, 0, 0, 1},
  });
  m_commandBuffer->BindPipeline(m_pipeline);

  uint32_t meshIndex = 0;
  for (auto entity : view) {
    auto &meshComponent = view.get<MeshComponent>(entity);
    auto &shadowComponent = view.get<ShadowComponent>(entity);
    const auto &meshImplData = meshComponent.m_impldata;
    auto &shadowImplData = shadowComponent.pointImplData;

    shadowImplData->descriptorSet->BindDynamicBuffer(2, m_meshDynamicUniformBuffer);

    if (meshImplData->vertexBuffer != nullptr && meshImplData->indexBuffer != nullptr) {
      auto size = sizeof(MeshComponent::UniformBufferBlockData);
      auto offset = sizeof(MeshComponent::UniformBufferBlockData) * meshIndex;

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
PointLightShadowMappingRenderPass::CreatePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  // depth pipeline
  auto depthShaderRes = shaderContainer->GetResource(m_shaderId);
  if (!depthShaderRes->IsLoad()) {
    depthShaderRes->LoadResource(m_rhiFactory, m_resourceManager.get());
  }
  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = 2048, .height = 2048});
  m_pipeline->SetVertexBufferLayout(GetMeshVertexInfoLayout(), VertexInputRate::VERTEX);
  m_pipeline->SetShader(depthShaderRes->GetShader());
  m_pipeline->SetPipelineLayout(GraphicsPipeLineLayout{
      .descriptorSetLayout = m_descriptorSetLayout,
  });
  m_pipeline->Create();
}

void
PointLightShadowMappingRenderPass::Execute(const Scene *scene,
                                           const ResourceManager *resourceManager) {
  auto view = Entity::GetAllEntity<MeshComponent, ShadowComponent>(scene);
  auto lightView = Entity::GetAllEntity<PointLightComponent>(const_cast<Scene *>(scene));

  for (auto mesh : view) {
    CreateBufferForEveryEntity(mesh, const_cast<Scene *>(scene));
  }

  auto bufferSize = view.size_hint() * sizeof(MeshComponent::UniformBufferBlockData);
  if (m_meshDynamicUniformBuffer == nullptr ||
      bufferSize != m_meshDynamicUniformBuffer->GetSize()) {
    RecordCommand(scene);
  }

  ClearDepth();
  SetUniformBuffer(scene);
  for (int i = 0; i < lightView.size(); i++) {
    SetUniformBufferForLight(scene, i);
    m_commandBuffer->SubmitCommand();
  }
}

}  // namespace Marbas
