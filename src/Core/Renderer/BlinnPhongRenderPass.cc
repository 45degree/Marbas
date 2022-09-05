#include "Core/Renderer/BlinnPhongRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "RHI/Interface/Pipeline.hpp"

namespace Marbas {

const String BlinnPhongRenderPass::renderPassName = "BlinnPhoneRenderPass";
const String BlinnPhongRenderPass::blinnPhongTargetName = "BlinnPhongTarget";

BlinnPhongRenderPassCreateInfo::BlinnPhongRenderPassCreateInfo() : DeferredRenderPassCreateInfo() {
  passName = BlinnPhongRenderPass::renderPassName;
  inputResource = {GeometryRenderPass::geometryTargetName};
  outputResource = {BlinnPhongRenderPass::blinnPhongTargetName,
                    GeometryRenderPass::depthTargetName};
}

BlinnPhongRenderPass::BlinnPhongRenderPass(const BlinnPhongRenderPassCreateInfo& createInfo)
    : DeferredRenderPass(createInfo) {}

void
BlinnPhongRenderPass::CreateRenderPass() {
  RenderPassCreateInfo renderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
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
  m_renderPass = m_rhiFactory->CreateRenderPass(renderPassCreateInfo);
}

void
BlinnPhongRenderPass::CreateShader() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/blinnPhong.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/blinnPhong.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);
}

void
BlinnPhongRenderPass::CreatePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto resource = shaderContainer->GetResource(m_shaderId);
  if (!resource->IsLoad()) {
    resource->LoadResource(m_rhiFactory, m_resourceManager.get());
  }

  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetShader(resource->GetShader());
  m_pipeline->SetVertexBufferLayout({}, VertexInputRate::INSTANCE);
  m_pipeline->SetVertexInputBindingDivisor({{0, 1}});
  m_pipeline->SetDepthStencilInfo(DepthStencilInfo{
      .depthTestEnable = false,
  });
  m_pipeline->Create();
}

void
BlinnPhongRenderPass::CreateDescriptorSetLayout() {
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = false,
      .bindingPoint = 0,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = false,
      .bindingPoint = 1,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = false,
      .bindingPoint = 2,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = true,
      .bindingPoint = 1,
  });
}

void
BlinnPhongRenderPass::OnInit() {
  m_vertexBuffer = m_rhiFactory->CreateVertexBuffer(0);
  m_descriptorSet = m_rhiFactory->CreateDescriptorSet(m_descriptorSetLayout);
  m_descriptorSet->BindBuffer(0, m_cameraUniformBuffer);

  auto bufferSize = sizeof(LightsInfo);
  m_lightUniformBuffer = m_rhiFactory->CreateUniformBuffer(bufferSize);
  m_descriptorSet->BindBuffer(1, m_lightUniformBuffer);

  // set input as image descriptor set
  auto gBuffer = m_inputTarget[GeometryRenderPass::geometryTargetName]->GetGBuffer();
  auto gColorBuffer = gBuffer->GetTexture(GBufferTexutreType::COLOR);
  auto gNormalBuffer = gBuffer->GetTexture(GBufferTexutreType::NORMALS);
  auto gPositionBuffer = gBuffer->GetTexture(GBufferTexutreType::POSITION);

  m_descriptorSet->BindImage(0, gColorBuffer);
  m_descriptorSet->BindImage(1, gNormalBuffer);
  m_descriptorSet->BindImage(2, gPositionBuffer);
}

void
BlinnPhongRenderPass::RecordCommand(const Scene* scene) {
  m_commandBuffer->Clear();

  // check framebuffer and renderpass
  DLOG_ASSERT(m_framebuffer != nullptr);
  DLOG_ASSERT(m_renderPass != nullptr);
  DLOG_ASSERT(m_pipeline != nullptr);

  // recreate uniform buffer

  auto bufferSize = sizeof(BlinnPhongRenderPass::LightsInfo);
  m_lightUniformBuffer->SetData(&m_uniformBufferBlock, bufferSize, 0);

  /**
   * set command
   */

  // set render pass command
  auto beginRenderPass = m_commandFactory->CreateBeginRenderPassCMD();
  beginRenderPass->SetRenderPass(m_renderPass);
  beginRenderPass->SetFrameBuffer(m_framebuffer);
  beginRenderPass->SetClearColor({0, 0, 0, 1});

  auto endRenderPass = m_commandFactory->CreateEndRenderPassCMD();
  endRenderPass->SetRenderPass(m_renderPass);
  endRenderPass->SetFrameBuffer(m_framebuffer);

  // set bind pipeline
  auto bindPipeline = m_commandFactory->CreateBindPipelineCMD();
  bindPipeline->SetPipeLine(m_pipeline);

  /**
   * begin to record command
   */

  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->AddCommand(std::move(beginRenderPass));
  m_commandBuffer->AddCommand(std::move(bindPipeline));

  auto bindVertexBuffer = m_commandFactory->CreateBindVertexBufferCMD();
  auto bindDescriptorSet = m_commandFactory->CreateBindDescriptorSetCMD();
  auto drawArray = m_commandFactory->CreateDrawArrayCMD(m_pipeline);

  bindVertexBuffer->SetVertexBuffer(m_vertexBuffer);
  drawArray->SetVertexCount(6);
  drawArray->SetInstanceCount(1);
  bindDescriptorSet->SetDescriptor(m_descriptorSet);

  m_commandBuffer->AddCommand(std::move(bindDescriptorSet));
  m_commandBuffer->AddCommand(std::move(drawArray));

  m_commandBuffer->AddCommand(std::move(endRenderPass));
  m_commandBuffer->EndRecordCmd();
}

void
BlinnPhongRenderPass::CreateFrameBuffer() {
  const auto& targetGBuffer = m_outputTarget[blinnPhongTargetName]->GetGBuffer();
  auto colorBuffer = targetGBuffer->GetTexture(GBufferTexutreType::COLOR);

  const auto& depthGBuffer = m_outputTarget[GeometryRenderPass::depthTargetName]->GetGBuffer();
  auto depthBuffer = depthGBuffer->GetTexture(GBufferTexutreType::DEPTH);

  if (colorBuffer == nullptr) {
    LOG(ERROR) << "can't get normal buffer or color buffer from the gbuffer";
    throw std::runtime_error("normal buffer and color buffer is needed by geometry render pass");
  }

  auto width = colorBuffer->GetWidth();
  auto height = colorBuffer->GetHeight();

  FrameBufferInfo createInfo{
      .width = width,
      .height = height,
      .renderPass = m_renderPass.get(),
      .attachments = {colorBuffer, depthBuffer},
  };

  m_framebuffer = m_rhiFactory->CreateFrameBuffer(createInfo);
}

void
BlinnPhongRenderPass::SetUniformBuffer(const Scene* scene) {
  auto paralleLightView = Entity::GetAllEntity<ParallelLightComponent>(scene);
  auto pointLightView = Entity::GetAllEntity<PointLightComponent>(scene);
  uint32_t index = 0;

  // set paralle light
  for (const auto& [entity, lightComponent] : paralleLightView.each()) {
    if (index >= maxLightsCount) {
      LOG(WARNING) << FORMAT("the max count of light is {}", maxLightsCount);
      break;
    }

    auto pos = lightComponent.m_light.GetPos();
    auto color = lightComponent.m_light.GetColor();
    auto viewPos = scene->GetEditorCamrea()->GetPosition();
    m_uniformBufferBlock.lights[index].pos = pos;
    m_uniformBufferBlock.lights[index].color = color;
    m_uniformBufferBlock.viewPos = viewPos;
    index++;
  }

  // set point light
  for (const auto& [entity, lightComponent] : pointLightView.each()) {
    if (index >= maxLightsCount) {
      LOG(WARNING) << FORMAT("the max count of light is {}", maxLightsCount);
      break;
    }
    auto pos = lightComponent.m_light.GetPos();
    auto color = lightComponent.m_light.GetColor();
    auto viewPos = scene->GetEditorCamrea()->GetPosition();
    m_uniformBufferBlock.lights[index].pos = pos;
    m_uniformBufferBlock.lights[index].color = color;
    m_uniformBufferBlock.viewPos = viewPos;
    index++;
  }

  m_uniformBufferBlock.lightsCount = index;

  auto bufferSize = sizeof(LightsInfo);
  m_lightUniformBuffer->SetData(&m_uniformBufferBlock, bufferSize, 0);
}

void
BlinnPhongRenderPass::Execute(const Scene* scene, const ResourceManager* resourceManager) {
  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }

  SetUniformBuffer(const_cast<Scene*>(scene));
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
