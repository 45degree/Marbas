#include "LightInjectPass.hpp"

#include "Core/Scene/GPUDataPipeline/LightGPUData.hpp"

namespace Marbas::GI {

LightInjectPass::LightInjectPass(const LightInjectPassCreateInfo& createInfo)
    : m_rhiFactory(createInfo.rhiFactory),
      m_voxelTexture(createInfo.voxelTexture),
      m_voxelNormal(createInfo.voxelNormal),
      m_voxelRadiance(createInfo.voxelRadiance) {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto bufCtx = m_rhiFactory->GetBufferContext();

  m_voxelInfoBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_voxelInfo, sizeof(VoxelInfo), false);

  SamplerCreateInfo samplerCreateInfo{
      .filter = Marbas::Filter::MIN_MAG_MIP_POINT,
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

  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
  m_set = pipelineCtx->CreateDescriptorSet(m_argument);
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_set,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_voxelInfoBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

LightInjectPass::~LightInjectPass() {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto bufCtx = m_rhiFactory->GetBufferContext();

  bufCtx->DestroyBuffer(m_voxelInfoBuffer);

  pipelineCtx->DestroySampler(m_sampler);
  pipelineCtx->DestroyDescriptorSet(m_set);
}

void
LightInjectPass::SetUp(RenderGraphComputeBuilder& builder) {
  builder.ReadTexture(m_voxelTexture, m_sampler);
  builder.ReadTexture(m_voxelNormal, m_sampler);
  builder.ReadStorageImage(m_voxelRadiance);

  auto& lightArgument = LightGPUData::GetDescriptorSetArgument();

  DescriptorSetArgument argument;
  argument.Bind(0, DescriptorType::IMAGE);
  argument.Bind(1, DescriptorType::IMAGE);
  argument.Bind(2, DescriptorType::STORAGE_IMAGE);

  builder.BeginPipeline();
  builder.AddShaderArgument(m_argument);
  builder.AddShaderArgument(argument);
  builder.AddShaderArgument(lightArgument);
  builder.SetShader("Shader/lightInject.comp.spv");
  builder.EndPipeline();
}

void
LightInjectPass::Execute(RenderGraphComputeRegistry& registry, ComputeCommandBuffer& commandBuffer) {
  auto pipeline = registry.GetPipeline(0);
  auto set = registry.GetInputDescriptorSet();
  auto lightSet = LightGPUData::GetLightSet();
  auto* radianceImage = registry.GetImage(m_voxelRadiance);

  commandBuffer.Begin();
  commandBuffer.ClearColorImage(radianceImage, {0, 0, 0, 0}, 0, 1, 0, 1);
  commandBuffer.BeginPipeline(pipeline);
  commandBuffer.BindDescriptorSet(pipeline, {m_set, set, lightSet});
  commandBuffer.Dispatch(256 / 8, 256 / 8, 256 / 8);
  commandBuffer.EndPipeline(pipeline);
  commandBuffer.End();
}

}  // namespace Marbas::GI
