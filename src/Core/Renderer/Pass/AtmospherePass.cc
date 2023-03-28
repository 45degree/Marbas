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

  RenderGraphPipelineCreateInfo createInfo;
  createInfo.SetPipelineLayout({
      DescriptorSetLayoutBinding{.bindingPoint = 0, .descriptorType = DescriptorType::UNIFORM_BUFFER},
      DescriptorSetLayoutBinding{.bindingPoint = 1, .descriptorType = DescriptorType::UNIFORM_BUFFER},
      DescriptorSetLayoutBinding{.bindingPoint = 0, .descriptorType = DescriptorType::IMAGE},
      DescriptorSetLayoutBinding{.bindingPoint = 1, .descriptorType = DescriptorType::IMAGE},
  });
  createInfo.AddShader(ShaderType::VERTEX_SHADER, "Shader/transmittanceLUT.vert.spv");
  createInfo.AddShader(ShaderType::FRAGMENT_SHADER, "Shader/atmosphere.frag.spv");
  createInfo.SetColorAttachmentsDesc({{
      .initAction = AttachmentInitAction::KEEP,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::COLOR_RENDER_TARGET | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::RGBA32F,
  }});
  createInfo.SetBlendConstance(0, 0, 0, 1);
  createInfo.SetBlendAttachments({BlendAttachment{.blendEnable = false}});
  DepthStencilCreateInfo depthStencil;
  depthStencil.depthTestEnable = false;
  createInfo.SetDepthStencil(depthStencil);

  builder.SetPipelineInfo(createInfo);
  builder.SetFramebufferSize(m_width, m_height, 1);
}

void
AtmospherePass::Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList) {
  auto* transmittanceLUTView = registry.GetRenderBackendTextureSubResource(m_transmittanceLUT, 0, 1, 0, 1);
  auto* multiScatterLUTView = registry.GetRenderBackendTextureSubResource(m_multiscatterLUT, 0, 1, 0, 1);

  auto& world = m_scene->GetWorld();
  auto view = world.view<EnvironmentComponent>();
  if (view.size() == 0) return;

  auto entity = view[0];
  auto& component = world.get<EnvironmentComponent>(entity);
  auto& physicalSky = component.physicalSky;

  // check framebuffer and renderpass
  commandList.SetDescriptorSetCount(1);

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
  m_atmosphereInfo.sunLuminace = directionLight.GetColor();
  m_atmosphereInfo.atmosphereHeight = physicalSky.atmosphereHeight;
  m_atmosphereInfo.rayleighScalarHeight = physicalSky.rayleighScalarHeight;
  m_atmosphereInfo.mieScalarHeight = physicalSky.mieScalarHeight;
  m_atmosphereInfo.mieAnisotropy = physicalSky.mieAnisotropy;
  m_atmosphereInfo.planetRadius = physicalSky.planetRadius;
  m_atmosphereInfo.ozoneCenterHeight = physicalSky.ozoneCenterHeight;
  m_atmosphereInfo.ozoneWidth = physicalSky.ozoneWidth;
  bufCtx->UpdateBuffer(m_atmosphereInfoBuffer, &m_atmosphereInfo, sizeof(AtmosphereInfo), 0);

  // bind resource
  RenderArgument argument;
  argument.BindUniformBuffer(0, m_cameraInfoUBO);
  argument.BindUniformBuffer(1, m_atmosphereInfoBuffer);
  argument.BindImage(0, m_sampler, transmittanceLUTView);
  argument.BindImage(1, m_sampler, multiScatterLUTView);

  /**
   * set command
   */
  commandList.Begin({{0, 0, 0, 0}, {1, 1}});
  commandList.BindArgument(argument);
  commandList.Draw(6, 1, 0, 0);
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
