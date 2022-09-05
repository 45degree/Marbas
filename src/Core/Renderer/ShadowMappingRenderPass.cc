#include "Core/Renderer/ShadowMappingRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Renderer/BlinnPhongRenderPass.hpp"
#include "Core/Renderer/DeferredRenderPass.hpp"
#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "Core/Scene/Component/ShadowComponent.hpp"
#include "Core/Scene/Component/TagComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "RHI/Interface/DescriptorSet.hpp"
#include "RHI/Interface/RenderPass.hpp"

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

ShadowMappingCreateInfo::ShadowMappingCreateInfo() : DeferredRenderPassCreateInfo() {
  passName = ShadowMappingRenderPass::renderPassName;
  inputResource = {
      GeometryRenderPass::geometryTargetName,
      BlinnPhongRenderPass::blinnPhongTargetName,
  };
  outputResource = {
      String(ShadowMappingRenderPass::renderTarget),
      GeometryRenderPass::depthTargetName,
  };
}

/**
 * shadow mapping render pass
 */

ShadowMappingRenderPass::ShadowMappingRenderPass(const ShadowMappingCreateInfo& createInfo)
    : DeferredRenderPass(createInfo) {
  /**
   * shadow depth render pass
   */
  m_depthCommandBuffer = m_commandFactory->CreateCommandBuffer();
  m_copyFrameCommandBuffer = m_commandFactory->CreateCommandBuffer();

  // create dynamic uniform buffer
  m_lightUniformBuffer = m_rhiFactory->CreateUniformBuffer(sizeof(LightUniformBlock));
  m_lightInfoUniformBuffer = m_rhiFactory->CreateUniformBuffer(sizeof(LightInfo));
}

void
ShadowMappingRenderPass::OnInit() {
  // create vertexBuffer and descriptorSet
  m_vertexBuffer = m_rhiFactory->CreateVertexBuffer(0);
  m_shadowDescriptorSet = m_rhiFactory->CreateDescriptorSet(m_descriptorSetLayout);
  m_shadowDescriptorSet->BindBuffer(0, m_cameraUniformBuffer);
  m_shadowDescriptorSet->BindBuffer(1, m_lightUniformBuffer);
  m_shadowDescriptorSet->BindBuffer(2, m_lightInfoUniformBuffer);

  const auto& geometryGBuffer = m_inputTarget[GeometryRenderPass::geometryTargetName]->GetGBuffer();
  const auto& blinnPhongGBuffer =
      m_inputTarget[BlinnPhongRenderPass::blinnPhongTargetName]->GetGBuffer();
  const auto& targetGBuffer = m_outputTarget[String(renderTarget)]->GetGBuffer();
  auto colorBuffer = targetGBuffer->GetTexture(GBufferTexutreType::COLOR);

  if (colorBuffer == nullptr) {
    LOG(ERROR) << "can't get normal buffer or color buffer from the gbuffer";
    throw std::runtime_error("normal buffer and color buffer is needed by geometry render pass");
  }

  // set input as image descriptor set
  auto gColorBuffer = blinnPhongGBuffer->GetTexture(GBufferTexutreType::COLOR);
  auto gPositionBuffer = geometryGBuffer->GetTexture(GBufferTexutreType::POSITION);
  auto gNormalBuffer = geometryGBuffer->GetTexture(GBufferTexutreType::NORMALS);

  m_shadowDescriptorSet->BindImage(0, gColorBuffer);
  m_shadowDescriptorSet->BindImage(1, m_depthTexture);
  m_shadowDescriptorSet->BindImage(2, gPositionBuffer);
  m_shadowDescriptorSet->BindImage(3, gNormalBuffer);
}

void
ShadowMappingRenderPass::CreateRenderPass() {
  // depth render pass
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

  // shadow render pass
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
}

void
ShadowMappingRenderPass::CreateShader() {
  auto shaderContainer = m_resourceManager->GetContainer<ShaderResource>();

  // depth shader
  auto depthShaderRes = shaderContainer->CreateResource();
  depthShaderRes->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/shadow_depth.vert.glsl");
  depthShaderRes->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/shadow_depth.frag.glsl");
  depthShaderRes->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_depthShaderId = shaderContainer->AddResource(depthShaderRes);

  // shadow shader
  auto shadowShaderRes = shaderContainer->CreateResource();
  shadowShaderRes->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/parallelShadow.vert.glsl");
  shadowShaderRes->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/parallelShadow.frag.glsl");
  shadowShaderRes->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shadowShaderRes);
}

void
ShadowMappingRenderPass::CreateFrameBuffer() {
  // depth frame buffer
  m_depthTexture = m_rhiFactory->CreateTexutre2D(4096, 4096, 1, TextureFormat::DEPTH);
  m_depthFrameBuffer = m_rhiFactory->CreateFrameBuffer(FrameBufferInfo{
      .width = 4096,
      .height = 4096,
      .renderPass = m_depthRenderPass.get(),
      .attachments = {m_depthTexture},
  });

  // shadow framebuffer
  const auto& depthGBuffer = m_outputTarget[GeometryRenderPass::depthTargetName]->GetGBuffer();
  auto depthBuffer = depthGBuffer->GetTexture(GBufferTexutreType::DEPTH);
  const auto& targetGBuffer = m_outputTarget[String(renderTarget)]->GetGBuffer();
  auto colorBuffer = targetGBuffer->GetTexture(GBufferTexutreType::COLOR);
  m_framebuffer = m_rhiFactory->CreateFrameBuffer(FrameBufferInfo{
      .width = m_width,
      .height = m_height,
      .renderPass = m_renderPass.get(),
      .attachments = {colorBuffer, depthBuffer},
  });
}

void
ShadowMappingRenderPass::CreateDescriptorSetLayout() {
  // depth binding layout
  m_depthDescriptorSetLayout = {
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

  // shadow binding layout
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
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // light info
      .isBuffer = true,
      .bindingPoint = 2,
  });
}

void
ShadowMappingRenderPass::CreatePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();

  // depth pipeline
  auto depthShaderRes = shaderContainer->GetResource(m_depthShaderId);
  if (!depthShaderRes->IsLoad()) {
    depthShaderRes->LoadResource(m_rhiFactory, m_resourceManager.get());
  }

  m_depthPipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_depthPipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = 4096, .height = 4096});
  m_depthPipeline->SetVertexBufferLayout(GetMeshVertexInfoLayout(), VertexInputRate::VERTEX);
  m_depthPipeline->SetShader(depthShaderRes->GetShader());
  m_depthPipeline->SetPipelineLayout(GraphicsPipeLineLayout{
      .descriptorSetLayout = m_depthDescriptorSetLayout,
  });
  m_depthPipeline->Create();

  // shadow pipeline
  auto shadowShaderRes = shaderContainer->GetResource(m_shaderId);
  if (!shadowShaderRes->IsLoad()) {
    shadowShaderRes->LoadResource(m_rhiFactory, m_resourceManager.get());
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
  m_pipeline->SetShader(shadowShaderRes->GetShader());
  m_pipeline->Create();
}

void
ShadowMappingRenderPass::SetUniformBuffer(const Scene* scene, const ParallelLight& light) {
  // get matrix
  const auto viewMatrix = light.GetViewMatrix();
  const auto perspectiveMatrix = light.GetProjectionMatrix();
  m_lightUniformBlock.view = viewMatrix;
  m_lightUniformBlock.projective = perspectiveMatrix;
  m_lightUniformBuffer->SetData(&m_lightUniformBlock, sizeof(LightUniformBlock), 0);

  m_lightInfo.pos = light.GetPos();
  m_lightInfoUniformBuffer->SetData(&m_lightInfo, sizeof(LightInfo), 0);

  uint32_t index = 0;
  auto view = Entity::GetAllEntity<MeshComponent, ShadowComponent>(const_cast<Scene*>(scene));
  for (auto entity : view) {
    auto& meshComponent = view.get<MeshComponent>(entity);

    if (meshComponent.m_impldata == nullptr) continue;
    auto offset = index * sizeof(MeshComponent::UniformBufferBlockData);
    auto size = sizeof(MeshComponent::UniformBufferBlockData);

    if (!meshComponent.m_model.expired()) {
      const auto model = meshComponent.m_model.lock();
      const auto& modelMatrix = model->GetModelMatrix();
      meshComponent.m_uniformBufferData.model = modelMatrix;
    }

    m_meshDynamicUniformBuffer->SetData(&meshComponent.m_uniformBufferData, size, offset);
    index++;
  }
}

void
ShadowMappingRenderPass::RecordCopyCommand() {
  /**
   * copy framebuffer command
   */
  m_copyFrameCommandBuffer->Clear();

  // get input target GBuffer
  const auto& inputTargetGBuffer =
      m_inputTarget[BlinnPhongRenderPass::blinnPhongTargetName]->GetGBuffer();
  auto inputColorTexture = inputTargetGBuffer->GetTexture(GBufferTexutreType::COLOR);

  const auto& outputTargetGBuffer = m_outputTarget[String(renderTarget)]->GetGBuffer();
  auto outputColorBuffer = outputTargetGBuffer->GetTexture(GBufferTexutreType::COLOR);

  auto copyTeture = m_commandFactory->CreateCopyImageToImageCMD();
  copyTeture->SetSrcImage(inputColorTexture);
  copyTeture->SetDstImage(outputColorBuffer);

  m_copyFrameCommandBuffer->BeginRecordCmd();
  m_copyFrameCommandBuffer->AddCommand(std::move(copyTeture));
  m_copyFrameCommandBuffer->EndRecordCmd();
}

void
ShadowMappingRenderPass::RecordCommand(const Scene* scene) {
  m_depthCommandBuffer->Clear();
  m_commandBuffer->Clear();

  // check framebuffer and renderpass
  DLOG_ASSERT(m_depthFrameBuffer != nullptr);
  DLOG_ASSERT(m_depthRenderPass != nullptr);
  DLOG_ASSERT(m_depthCommandBuffer != nullptr);

  auto view = Entity::GetAllEntity<MeshComponent, ShadowComponent>(const_cast<Scene*>(scene));
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
    auto& meshComponent = view.get<MeshComponent>(entity);
    auto& shadowComponent = view.get<ShadowComponent>(entity);
    const auto& meshImplData = meshComponent.m_impldata;
    auto& shadowImplData = shadowComponent.implData;

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
ShadowMappingRenderPass::CreateBufferForEveryEntity(const entt::entity& entity, Scene* scene) {
  DLOG_ASSERT(Entity::HasComponent<ShadowComponent>(scene, entity));

  auto& shadowComponent = Entity::GetComponent<ShadowComponent>(scene, entity);
  if (shadowComponent.implData != nullptr) return;

  auto implData = std::make_shared<ShadowComponent_Impl>();

  implData->descriptorSet = m_rhiFactory->CreateDescriptorSet(m_depthDescriptorSetLayout);
  implData->descriptorSet->BindBuffer(0, m_cameraUniformBuffer);
  implData->descriptorSet->BindBuffer(1, m_lightUniformBuffer);

  shadowComponent.implData = implData;
  m_needToRecordComand = true;
}

void
ShadowMappingRenderPass::Execute(const Scene* scene, const ResourceManager* resourceManager) {
  auto view = Entity::GetAllEntity<MeshComponent, ShadowComponent>(scene);
  auto lightView = Entity::GetAllEntity<ParallelLightComponent>(const_cast<Scene*>(scene));

  // there is no need to draw the shadow if the scene don't have any meshes

  if (m_needToRecordCopyCommand) {
    RecordCopyCommand();
    m_needToRecordCopyCommand = false;
  }
  m_copyFrameCommandBuffer->SubmitCommand();

  for (auto light : lightView) {
    auto& lightComponent = lightView.get<ParallelLightComponent>(light);
    for (auto mesh : view) {
      CreateBufferForEveryEntity(mesh, const_cast<Scene*>(scene));
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
