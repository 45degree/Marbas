#include "AtmospherePass.hpp"

#include <glog/logging.h>

#include "Core/Scene/Component/EnvironmentComponent.hpp"
#include "Core/Scene/Component/LightComponent.hpp"

namespace Marbas {

AtmospherePass::AtmospherePass(const AtmospherePassCreateInfo& createInfo)
    : m_width(createInfo.width),
      m_height(createInfo.height),
      m_scene(createInfo.scene),
      m_rhiFactory(createInfo.rhiFactory),
      m_transmittanceLUT(createInfo.transmittanceLUT),
      m_multiscatterLUT(createInfo.multiscatterLUT),
      m_finalColorTexture(createInfo.colorTexture) {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  SamplerCreateInfo samplerCreateInfo{
      .filter = Marbas::Filter::MIN_MAG_MIP_LINEAR,
      .addressU = Marbas::SamplerAddressMode::WRAP,
      .addressV = Marbas::SamplerAddressMode::WRAP,
      .addressW = Marbas::SamplerAddressMode::WRAP,
      .comparisonOp = Marbas::ComparisonOp::ALWAYS,
      .mipLodBias = 0,
      .minLod = 0,
      .maxLod = 0,
      .borderColor = Marbas::BorderColor::IntOpaqueBlack,
  };
  m_sampler = pipelineCtx->CreateSampler(samplerCreateInfo);

  m_cameraInfoUBO = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_cameraInfo, sizeof(CameraInfo), true);
  m_atmosphereInfoBuffer =
      bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_atmosphereInfo, sizeof(AtmosphereInfo), false);

  // descriptor set

  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
  m_argument.Bind(1, DescriptorType::UNIFORM_BUFFER);
  m_inputArgument.Bind(0, DescriptorType::IMAGE);
  m_inputArgument.Bind(1, DescriptorType::IMAGE);

  m_descriptorSet = pipelineCtx->CreateDescriptorSet(m_argument);
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_descriptorSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_cameraInfoUBO,
      .offset = 0,
      .arrayElement = 0,
  });
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_descriptorSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 1,
      .buffer = m_atmosphereInfoBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

AtmospherePass::~AtmospherePass() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  pipelineCtx->DestroySampler(m_sampler);
  bufCtx->DestroyBuffer(m_vertexBuffer);
  bufCtx->DestroyBuffer(m_indexBuffer);
  bufCtx->DestroyBuffer(m_cameraInfoUBO);
  bufCtx->DestroyBuffer(m_atmosphereInfoBuffer);
}

void
AtmospherePass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.WriteTexture(m_finalColorTexture);
  builder.ReadTexture(m_transmittanceLUT, m_sampler);
  builder.ReadTexture(m_multiscatterLUT, m_sampler);

  builder.SetFramebufferSize(m_width, m_height, 1);

  // RenderGraphPipelineCreateInfo createInfo;
  builder.BeginPipeline();
  builder.AddShaderArgument(m_argument);
  builder.AddShaderArgument(m_inputArgument);
  builder.AddShader("Shader/transmittanceLUT.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/atmosphere.frag.spv", ShaderType::FRAGMENT_SHADER);
  builder.AddColorTarget({
      .initAction = AttachmentInitAction::KEEP,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::COLOR_RENDER_TARGET | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::RGBA32F,
  });
  builder.SetBlendConstant(0, 0, 0, 1);
  builder.AddBlendAttachments({.blendEnable = false});
  builder.EnableDepthTest(false);
  builder.EndPipeline();
}

void
AtmospherePass::Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandList) {
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();
  auto inputSet = registry.GetInputDescriptorSet();

  auto& world = m_scene->GetWorld();
  auto view = world.view<EnvironmentComponent>();
  if (view.size() == 0) return;

  auto entity = view[0];
  auto& component = world.get<EnvironmentComponent>(entity);
  auto& physicalSky = component.physicalSky;

  // set camera info
  auto camera = m_scene->GetEditorCamera();
  auto* bufCtx = m_rhiFactory->GetBufferContext();
  m_cameraInfo.view = camera->GetViewMatrix();
  m_cameraInfo.projection = camera->GetProjectionMatrix();
  bufCtx->UpdateBuffer(m_cameraInfoUBO, &m_cameraInfo, sizeof(CameraInfo), 0);

  // set atmosphere infomation
  auto sunView = world.view<DirectionLightComponent, SunLightTag>();
  LOG_IF(WARNING, sunView.size_hint() > 1) << "find multi sun, use the first sun";

  auto sun = *sunView.begin();
  auto& directionLight = world.get<DirectionLightComponent>(sun).m_light;

  m_atmosphereInfo.lightDir = directionLight.GetDirection();
  m_atmosphereInfo.lightColor = directionLight.GetColor();
  m_atmosphereInfo.sunLuminace = directionLight.GetEnergy();
  m_atmosphereInfo.atmosphereHeight = physicalSky.atmosphereHeight;
  m_atmosphereInfo.rayleighScalarHeight = physicalSky.rayleighScalarHeight;
  m_atmosphereInfo.mieScalarHeight = physicalSky.mieScalarHeight;
  m_atmosphereInfo.mieAnisotropy = physicalSky.mieAnisotropy;
  m_atmosphereInfo.planetRadius = physicalSky.planetRadius;
  m_atmosphereInfo.ozoneCenterHeight = physicalSky.ozoneCenterHeight;
  m_atmosphereInfo.ozoneWidth = physicalSky.ozoneWidth;
  bufCtx->UpdateBuffer(m_atmosphereInfoBuffer, &m_atmosphereInfo, sizeof(AtmosphereInfo), 0);

  /**
   * record command
   */
  std::array<ViewportInfo, 1> viewport;
  viewport[0].x = 0;
  viewport[0].y = 0;
  viewport[0].width = m_width;
  viewport[0].height = m_height;
  viewport[0].minDepth = 0;
  viewport[0].maxDepth = 1;

  std::array<ScissorInfo, 1> scissor;
  scissor[0].x = 0;
  scissor[0].y = 0;
  scissor[0].height = m_height;
  scissor[0].width = m_width;

  commandList.Begin();
  commandList.BeginPipeline(pipeline, framebuffer, {{0, 0, 0, 0}, {1, 1}});
  commandList.SetViewports(viewport);
  commandList.SetScissors(scissor);
  commandList.BindDescriptorSet(pipeline, {m_descriptorSet, inputSet});
  commandList.Draw(6, 1, 0, 0);
  commandList.EndPipeline(pipeline);
  commandList.End();
}

bool
AtmospherePass::IsEnable() {
  auto& world = m_scene->GetWorld();
  auto view = world.view<EnvironmentComponent>();
  if (view.size() == 0) return false;

  auto entity = view[0];
  auto& component = world.get<EnvironmentComponent>(entity);
  if (component.currentItem != EnvironmentComponent::physicalSkyItem) return false;

  // check sun
  auto sunView = world.view<DirectionLightComponent, SunLightTag>();
  return sunView.size_hint() > 0;
}

}  // namespace Marbas
