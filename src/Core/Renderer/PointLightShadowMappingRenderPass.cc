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
      String(ShadowMappingRenderPass::renderTarget),
  };
  outputResource = {
      String(PointLightShadowMappingRenderPass::targetName),
      GeometryRenderPass::depthTargetName,
  };
}

/**
 * point light shadow mapping render pass
 */

PointLightShadowMappingRenderPass::PointLightShadowMappingRenderPass(const CreateInfo &createInfo)
    : DeferredRenderPass(createInfo) {
  // create command Factory and load shader container
  m_commandFactory = m_rhiFactory->CreateCommandFactory();
  auto shaderContainer = m_resourceManager->GetContainer<ShaderResource>();

  /**
   * shadow depth render pass
   */
  RenderPassCreateInfo depthRenderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
                  .format = TextureFormat::DEPTH,
                  .type = AttachmentType::Depth,
                  .loadOp = AttachmentLoadOp::Clear,
              },
          },
  };
  m_depthRenderPass = m_rhiFactory->CreateRenderPass(depthRenderPassCreateInfo);
  m_depthPipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_depthCommandBuffer = m_commandFactory->CreateCommandBuffer();

  // read shader
  auto depthShaderRes = shaderContainer->CreateResource();
  depthShaderRes->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/pointLightDepth.vert.glsl");
  depthShaderRes->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/pointLightDepth.frag.glsl");
  depthShaderRes->SetShaderStage(ShaderType::GEOMETRY_SHADER, "Shader/pointLightDepth.gemo.glsl");
  depthShaderRes->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_depthShaderId = shaderContainer->AddResource(depthShaderRes);

  // set depth descriptor set layout
  m_depthDescriptorSetLayout = {
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

  // create pipeline
  m_depthPipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_depthPipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = 2048, .height = 2048});
  m_depthPipeline->SetVertexBufferLayout(GetMeshVertexInfoLayout(), VertexInputRate::VERTEX);
  m_depthPipeline->SetShader(depthShaderRes->GetShader());
  m_depthPipeline->SetPipelineLayout(GraphicsPipeLineLayout{
      .descriptorSetLayout = m_depthDescriptorSetLayout,
  });
  m_depthPipeline->Create();

  // create dynamic uniform buffer
  m_lightInfoUniformBuffer = m_rhiFactory->CreateUniformBuffer(sizeof(LightInfo));

  // create command buffer
  m_depthCommandBuffer = m_commandFactory->CreateCommandBuffer();

  /**
   * shadow render pass
   */
  RenderPassCreateInfo shadowRenderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
                  // output color
                  .format = TextureFormat::RGBA,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
              AttachmentDescription{
                  .format = TextureFormat::DEPTH,
                  .type = AttachmentType::Depth,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
          },
  };
  m_renderPass = m_rhiFactory->CreateRenderPass(shadowRenderPassCreateInfo);

  // read shader
  auto shadowShaderRes = shaderContainer->CreateResource();
  shadowShaderRes->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/pointLightShadow.vert.glsl");
  shadowShaderRes->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/pointLightShadow.frag.glsl");
  shadowShaderRes->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shadowShaderRes);

  // set descriptor set layout
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // color texture
      .isBuffer = false,
      .bindingPoint = 0,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // depth texture
      .isBuffer = false,
      .bindingPoint = 1,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // position gbuffer
      .isBuffer = false,
      .bindingPoint = 2,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // normal gbuffer
      .isBuffer = false,
      .bindingPoint = 3,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // light matrix
      .isBuffer = true,
      .bindingPoint = 1,
  });

  GeneratePipeline();

  // create vertexBuffer and descriptorSet
  m_vertexBuffer = m_rhiFactory->CreateVertexBuffer(0);
  m_shadowDescriptorSet = GenerateDescriptorSet();
  BindCameraUniformBuffer(m_shadowDescriptorSet.get());
  m_shadowDescriptorSet->BindBuffer(1, m_lightInfoUniformBuffer);
}

void
PointLightShadowMappingRenderPass::CreateFrameBuffer() {
  m_depthTexture = m_rhiFactory->CreateTextureCubeMap(2048, 2048, TextureFormat::DEPTH);
  m_depthFrameBuffer = m_rhiFactory->CreateFrameBuffer(FrameBufferInfo{
      .width = 2048,
      .height = 2048,
      .renderPass = m_depthRenderPass.get(),
      .attachments = {m_depthTexture},
  });

  const auto &depthGBuffer = m_outputTarget[GeometryRenderPass::depthTargetName]->GetGBuffer();
  auto depthBuffer = depthGBuffer->GetTexture(GBufferTexutreType::DEPTH);
  const auto &targetGBuffer = m_outputTarget[String(targetName)]->GetGBuffer();
  auto colorBuffer = targetGBuffer->GetTexture(GBufferTexutreType::COLOR);
  m_framebuffer = m_rhiFactory->CreateFrameBuffer(FrameBufferInfo{
      .width = m_width,
      .height = m_height,
      .renderPass = m_renderPass.get(),
      .attachments = {colorBuffer, depthBuffer},
  });

  const auto &geometryGBuffer = m_inputTarget[GeometryRenderPass::geometryTargetName]->GetGBuffer();
  const auto &paralleLightGBuffer =
      m_inputTarget[String(ShadowMappingRenderPass::renderTarget)]->GetGBuffer();

  if (colorBuffer == nullptr) {
    LOG(ERROR) << "can't get normal buffer or color buffer from the gbuffer";
    throw std::runtime_error("normal buffer and color buffer is needed by geometry render pass");
  }

  // set input as image descriptor set
  auto gColorBuffer = paralleLightGBuffer->GetTexture(GBufferTexutreType::COLOR);
  auto gPositionBuffer = geometryGBuffer->GetTexture(GBufferTexutreType::POSITION);
  auto gNormalBuffer = geometryGBuffer->GetTexture(GBufferTexutreType::NORMALS);

  m_shadowDescriptorSet->BindImage(0, gColorBuffer);
  m_shadowDescriptorSet->BindImage(1, m_depthTexture);
  m_shadowDescriptorSet->BindImage(2, gPositionBuffer);
  m_shadowDescriptorSet->BindImage(3, gNormalBuffer);
}

void
PointLightShadowMappingRenderPass::CreateBufferForEveryEntity(const entt::entity &entity,
                                                              Scene *scene) {
  DLOG_ASSERT(Entity::HasComponent<ShadowComponent>(scene, entity));

  auto &shadowComponent = Entity::GetComponent<ShadowComponent>(scene, entity);
  if (shadowComponent.implData != nullptr) return;

  auto implData = std::make_shared<ShadowComponent_Impl>();
  implData->descriptorSet = m_rhiFactory->CreateDescriptorSet(m_depthDescriptorSetLayout);
  implData->descriptorSet->BindBuffer(0, m_cameraUniformBuffer);
  implData->descriptorSet->BindBuffer(1, m_lightInfoUniformBuffer);

  shadowComponent.implData = implData;
  m_needToRecordComand = true;
}

void
PointLightShadowMappingRenderPass::SetUniformBuffer(const Scene *scene, const PointLight &light) {
  for (int i = 0; i < 6; i++) {
    m_lightInfo.matrixes[i] = light.GetViewMatrix(i);
  }
  m_lightInfo.projectMatrix = light.GetProjectionMatrix();
  m_lightInfo.pos = light.GetPos();
  m_lightInfo.farPlane = light.GetFarPlane();
  m_lightInfoUniformBuffer->SetData(&m_lightInfo, sizeof(LightInfo), 0);

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
PointLightShadowMappingRenderPass::RecordCommand(const Scene *scene) {
  m_depthCommandBuffer->Clear();
  m_commandBuffer->Clear();

  // check framebuffer and renderpass
  DLOG_ASSERT(m_depthFrameBuffer != nullptr)
      << FORMAT("{}'s framebuffer is null, can't record command ",
                NAMEOF_TYPE(PointLightShadowMappingRenderPass));

  DLOG_ASSERT(m_depthRenderPass != nullptr)
      << FORMAT("{}'s render pass is null, can't record command ",
                NAMEOF_TYPE(PointLightShadowMappingRenderPass));

  DLOG_ASSERT(m_depthCommandBuffer != nullptr)
      << FORMAT("{}'s pipeline is null, can't record command",
                NAMEOF_TYPE(PointLightShadowMappingRenderPass));

  auto view = Entity::GetAllEntity<MeshComponent, ShadowComponent>(const_cast<Scene *>(scene));
  auto entityCount = view.size_hint();

  // recreate dynamic uniform buffer
  auto bufferSize = entityCount * sizeof(MeshComponent::UniformBufferBlockData);
  m_meshDynamicUniformBuffer = m_rhiFactory->CreateDynamicUniforBuffer(bufferSize);

  /**
   * set shadow depth command
   */
  auto beginDepthRenderPass = m_commandFactory->CreateBeginRenderPassCMD();
  beginDepthRenderPass->SetRenderPass(m_depthRenderPass);
  beginDepthRenderPass->SetFrameBuffer(m_depthFrameBuffer);
  beginDepthRenderPass->SetClearColor({0, 0, 0, 1});

  auto endDepthRenderPass = m_commandFactory->CreateEndRenderPassCMD();
  endDepthRenderPass->SetFrameBuffer(m_depthFrameBuffer);

  // set bind pipeline
  auto bindDepthPipeline = m_commandFactory->CreateBindPipelineCMD();
  bindDepthPipeline->SetPipeLine(m_depthPipeline);

  // record command
  m_depthCommandBuffer->BeginRecordCmd();
  m_depthCommandBuffer->AddCommand(std::move(beginDepthRenderPass));
  m_depthCommandBuffer->AddCommand(std::move(bindDepthPipeline));

  uint32_t meshIndex = 0;
  for (auto entity : view) {
    auto &meshComponent = view.get<MeshComponent>(entity);
    auto &shadowComponent = view.get<ShadowComponent>(entity);
    const auto &meshImplData = meshComponent.m_impldata;
    auto &shadowImplData = shadowComponent.implData;

    shadowImplData->descriptorSet->BindDynamicBuffer(2, m_meshDynamicUniformBuffer);

    if (meshImplData->vertexBuffer != nullptr && meshImplData->indexBuffer != nullptr) {
      auto bindVertexBuffer = m_commandFactory->CreateBindVertexBufferCMD();
      auto bindIndexBuffer = m_commandFactory->CreateBindIndexBufferCMD();
      auto bindDescriptorSet = m_commandFactory->CreateBindDescriptorSetCMD();
      auto drawIndex = m_commandFactory->CreateDrawIndexCMD(m_depthPipeline);

      bindVertexBuffer->SetVertexBuffer(meshImplData->vertexBuffer);
      bindIndexBuffer->SetIndexBuffer(meshImplData->indexBuffer);
      auto size = sizeof(MeshComponent::UniformBufferBlockData);
      auto offset = sizeof(MeshComponent::UniformBufferBlockData) * meshIndex;
      bindDescriptorSet->SetDescriptor(shadowImplData->descriptorSet);
      bindDescriptorSet->SetDynamicDescriptorBufferPiece({
          DynamicBufferPiece{
              .offset = static_cast<uint32_t>(offset),
              .size = static_cast<uint32_t>(size),
          },
      });
      drawIndex->SetIndexCount(meshImplData->indexBuffer->GetIndexCount());

      m_depthCommandBuffer->AddCommand(std::move(bindVertexBuffer));
      m_depthCommandBuffer->AddCommand(std::move(bindIndexBuffer));
      m_depthCommandBuffer->AddCommand(std::move(bindDescriptorSet));
      m_depthCommandBuffer->AddCommand(std::move(drawIndex));

      meshIndex++;
    }
  }
  m_depthCommandBuffer->AddCommand(std::move(endDepthRenderPass));
  m_depthCommandBuffer->EndRecordCmd();

  /**
   * set shadow command
   */

  auto beginRenderPass = m_commandFactory->CreateBeginRenderPassCMD();
  beginRenderPass->SetRenderPass(m_renderPass);
  beginRenderPass->SetFrameBuffer(m_framebuffer);

  auto endRenderPass = m_commandFactory->CreateEndRenderPassCMD();
  endRenderPass->SetFrameBuffer(m_framebuffer);

  auto bindPipeline = m_commandFactory->CreateBindPipelineCMD();
  bindPipeline->SetPipeLine(m_pipeline);

  // record command
  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->AddCommand(std::move(beginRenderPass));
  m_commandBuffer->AddCommand(std::move(bindPipeline));

  auto bindVertexBuffer = m_commandFactory->CreateBindVertexBufferCMD();
  auto bindDescriptorSet = m_commandFactory->CreateBindDescriptorSetCMD();
  auto drawArray = m_commandFactory->CreateDrawArrayCMD(m_pipeline);

  bindVertexBuffer->SetVertexBuffer(m_vertexBuffer);
  bindDescriptorSet->SetDescriptor(m_shadowDescriptorSet);
  drawArray->SetVertexCount(6);
  drawArray->SetInstanceCount(1);

  m_commandBuffer->AddCommand(std::move(bindVertexBuffer));
  m_commandBuffer->AddCommand(std::move(bindDescriptorSet));
  m_commandBuffer->AddCommand(std::move(drawArray));

  m_commandBuffer->EndRecordCmd();
}

void
PointLightShadowMappingRenderPass::GeneratePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto resource = shaderContainer->GetResource(m_shaderId);
  if (!resource->IsLoad()) {
    resource->LoadResource(m_rhiFactory, m_resourceManager.get());
  }

  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetVertexBufferLayout({}, VertexInputRate::INSTANCE);
  m_pipeline->SetVertexInputBindingDivisor({{0, 1}});
  m_pipeline->SetDepthStencilInfo(DepthStencilInfo{
      .depthTestEnable = false,
  });
  m_pipeline->SetPipelineLayout(GraphicsPipeLineLayout{
      .descriptorSetLayout = m_descriptorSetLayout,
  });
  m_pipeline->SetShader(resource->GetShader());
  m_pipeline->Create();
}

void
PointLightShadowMappingRenderPass::Execute(const Scene *scene,
                                           const ResourceManager *resourceManager) {
  auto view = Entity::GetAllEntity<MeshComponent, ShadowComponent>(scene);
  auto lightView = Entity::GetAllEntity<PointLightComponent>(const_cast<Scene *>(scene));

  // there is no need to draw the shadow if the scene don't have any meshes

  for (auto light : lightView) {
    auto &lightComponent = lightView.get<PointLightComponent>(light);
    for (auto mesh : view) {
      CreateBufferForEveryEntity(mesh, const_cast<Scene *>(scene));
    }

    if (m_needToRecordComand) {
      RecordCommand(scene);
      m_needToRecordComand = false;
    }

    SetUniformBuffer(scene, lightComponent.m_light);

    m_depthCommandBuffer->SubmitCommand();
    m_commandBuffer->SubmitCommand();
  }
}

}  // namespace Marbas
