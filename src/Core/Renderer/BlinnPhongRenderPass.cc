#include "Core/Renderer/BlinnPhongRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "RHI/Interface/Pipeline.hpp"

namespace Marbas {

const String BlinnPhongRenderPass::renderPassName = "BlinnPhoneRenderPass";
const String BlinnPhongRenderPass::blinnPhongTargetName = "BlinnPhongTarget";

BlinnPhongRenderPassCreateInfo::BlinnPhongRenderPassCreateInfo()
    : DeferredRenderPassNodeCreateInfo() {
  passName = BlinnPhongRenderPass::renderPassName;
  inputResource = {GeometryRenderPass::geometryTargetName};
  outputResource = {BlinnPhongRenderPass::blinnPhongTargetName,
                    GeometryRenderPass::depthTargetName};
}

BlinnPhongRenderPass::BlinnPhongRenderPass(const BlinnPhongRenderPassCreateInfo& createInfo)
    : DeferredRenderPass(createInfo) {
  DLOG_ASSERT(m_rhiFactory != nullptr)
      << FORMAT("can't Initialize the {}, because the rhiFactory isn't been set",
                NAMEOF_TYPE(BlinnPhongRenderPass));

  DLOG_ASSERT(m_resourceManager != nullptr)
      << FORMAT("can't Initialize the {}, because the resource manager isn't been set",
                NAMEOF_TYPE(BlinnPhongRenderPass));

  /**
   * set render pass and pipeline
   */

  // create render pass
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

  // create command factory
  m_commandFactory = m_rhiFactory->CreateCommandFactory();
  m_commandBuffer = m_commandFactory->CreateCommandBuffer();

  // read shader
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/blinnPhong.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/blinnPhong.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);

  // create pipeline
  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetShader(shaderResource->GetShader());
  m_pipeline->SetVertexBufferLayout({}, VertexInputRate::INSTANCE);
  m_pipeline->SetVertexInputBindingDivisor({{0, 1}});
  m_pipeline->SetDepthStencilInfo(DepthStencilInfo{
      .depthTestEnable = false,
  });
  m_pipeline->Create();

  // create uniform buffer
  m_vertexBuffer = m_rhiFactory->CreateVertexBuffer(0);
  m_uniformBuffer = m_rhiFactory->CreateUniformBuffer(sizeof(LightsInfo));
  DescriptorSetInfo descriptorSetInfo{
      DescriptorInfo{
          .isBuffer = true,
          .type = BufferDescriptorType::UNIFORM_BUFFER,
          .bindingPoint = 0,
      },
      DescriptorInfo{
          .isBuffer = false,
          .bindingPoint = 0,
      },
      DescriptorInfo{
          .isBuffer = false,
          .bindingPoint = 1,
      },
      DescriptorInfo{
          .isBuffer = false,
          .bindingPoint = 2,
      },
  };
  m_descriptorSet = m_rhiFactory->CreateDescriptorSet(descriptorSetInfo);
  m_descriptorSet->BindBuffer(0, m_uniformBuffer->GetIBufferDescriptor());
}

void
BlinnPhongRenderPass::RecordCommand(const Scene* scene) {
  m_commandBuffer->Clear();

  // check framebuffer and renderpass
  DLOG_ASSERT(m_framebuffer != nullptr) << FORMAT("{}'s framebuffer is null, can't record command",
                                                  NAMEOF_TYPE(BlinnPhongRenderPass));

  DLOG_ASSERT(m_renderPass != nullptr) << FORMAT("{}'s render pass is null, can't record command",
                                                 NAMEOF_TYPE(BlinnPhongRenderPass));

  DLOG_ASSERT(m_pipeline != nullptr)
      << FORMAT("{}'s pipeline is null, can't record command", NAMEOF_TYPE(BlinnPhongRenderPass));

  // recreate uniform buffer

  auto bufferSize = sizeof(BlinnPhongRenderPass::LightsInfo);
  m_uniformBuffer->SetData(&m_uniformBufferBlock, bufferSize, 0);

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

  // set input as image descriptor set
  auto gBuffer = m_inputTarget[GeometryRenderPass::geometryTargetName]->GetGBuffer();
  auto gColorBuffer = gBuffer->GetTexture(GBufferTexutreType::COLOR);
  auto gNormalBuffer = gBuffer->GetTexture(GBufferTexutreType::NORMALS);
  auto gPositionBuffer = gBuffer->GetTexture(GBufferTexutreType::POSITION);

  m_descriptorSet->BindImage(0, gColorBuffer->GetDescriptor());
  m_descriptorSet->BindImage(1, gNormalBuffer->GetDescriptor());
  m_descriptorSet->BindImage(2, gPositionBuffer->GetDescriptor());
}

void
BlinnPhongRenderPass::SetUniformBuffer(const Scene* scene) {
  auto view = Entity::GetAllEntity<LightComponent>(scene);
  uint32_t index = 0;
  for (const auto& [entity, lightComponent] : view.each()) {
    if (index >= maxLightsCount) {
      LOG(WARNING) << FORMAT("the max count of light is {}", maxLightsCount);
      break;
    }

    auto pos = lightComponent.m_light->GetPos();
    auto color = lightComponent.m_light->GetColor();
    auto viewPos = scene->GetEditorCamrea()->GetPosition();
    m_uniformBufferBlock.lights[index].pos = pos;
    m_uniformBufferBlock.lights[index].color = color;
    m_uniformBufferBlock.viewPos = viewPos;
    index++;
  }
  m_uniformBufferBlock.lightsCount = index;

  auto bufferSize = sizeof(LightsInfo);
  m_uniformBuffer->SetData(&m_uniformBufferBlock, bufferSize, 0);
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
