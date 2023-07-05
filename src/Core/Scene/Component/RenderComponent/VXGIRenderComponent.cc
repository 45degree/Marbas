#include "VXGIRenderComponent.hpp"

#include <cmath>

#include "Image.hpp"
#include "PipelineContext.hpp"

namespace Marbas {

VXGIGlobalComponent::VXGIGlobalComponent(RHIFactory* rhiFactory) : m_rhiFactory(rhiFactory) {
  using enum BufferType;

  auto* bufCtx = rhiFactory->GetBufferContext();
  auto* pipelineCtx = rhiFactory->GetPipelineContext();

  m_infoBuffer = bufCtx->CreateBuffer(UNIFORM_BUFFER, &m_voxelsInfo, sizeof(VoxelInfo), false);
  m_set = pipelineCtx->CreateDescriptorSet(GetDescriptorSetArgument());

  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_set,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 1,
      .buffer = m_infoBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

VXGIGlobalComponent::~VXGIGlobalComponent() {
  auto* bufCtx = m_rhiFactory->GetBufferContext();
  auto* pipelineCtx = m_rhiFactory->GetPipelineContext();

  bufCtx->DestroyBuffer(m_infoBuffer);
  pipelineCtx->DestroyDescriptorSet(m_set);
}

const DescriptorSetArgument&
VXGIGlobalComponent::GetDescriptorSetArgument() {
  static DescriptorSetArgument argument({
      {0, DescriptorType::IMAGE},
      {1, DescriptorType::UNIFORM_BUFFER},
  });

  return argument;
}

void
VXGIGlobalComponent::BindVoxelProbe(const VoxelRenderComponent& component, uint32_t index) {
  m_voxelsInfo.m_voxelInfo[index].pos = component.m_voxelInfo.pos;
  m_voxelsInfo.m_voxelInfo[index].voxelSizeResolution = component.m_voxelInfo.voxelSizeResolution;

  auto* pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto* bufCtx = m_rhiFactory->GetBufferContext();

  m_voxelRadiances[index] = component.m_voxelRadiance;
  pipelineCtx->BindImage(BindImageInfo{
      .descriptorSet = m_set,
      .bindingPoint = static_cast<uint16_t>(index),
      .imageView = component.m_voxelRadianceView,
      .sampler = component.m_radianceSampler,
  });
  bufCtx->UpdateBuffer(m_infoBuffer, &m_voxelsInfo, sizeof(VoxelInfo), 0);
}

void
VXGIGlobalComponent::UpdateVoxelProbe(const VoxelRenderComponent& component, uint32_t index) {
  m_voxelsInfo.m_voxelInfo[index].pos = component.m_voxelInfo.pos;
  m_voxelsInfo.m_voxelInfo[index].voxelSizeResolution = component.m_voxelInfo.voxelSizeResolution;

  auto* pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto* bufCtx = m_rhiFactory->GetBufferContext();
  bufCtx->UpdateBuffer(m_infoBuffer, &m_voxelsInfo, sizeof(VoxelInfo), 0);
}

VoxelRenderComponent::VoxelRenderComponent(RHIFactory* rhiFactory, const glm::vec3& size, uint32_t resolution)
    : m_rhiFactory(rhiFactory), m_resolution(resolution) {
  using enum BufferType;

  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  ImageCreateInfo createInfo;
  createInfo.sampleCount = SampleCount::BIT1;
  createInfo.usage = ImageUsageFlags::STORAGE | ImageUsageFlags::SHADER_READ;
  createInfo.width = resolution;
  createInfo.height = resolution;
  createInfo.format = ImageFormat::RGBA;
  createInfo.imageDesc = Image3DDesc{.depth = resolution};
  createInfo.mipMapLevel = 1;

  // m_voxelStaticDiffuse = bufCtx->CreateImage(createInfo);
  // m_voxelStaticNormal = bufCtx->CreateImage(createInfo);
  m_voxelDiffuse = bufCtx->CreateImage(createInfo);
  m_voxelNormal = bufCtx->CreateImage(createInfo);

  createInfo.mipMapLevel = static_cast<uint32_t>(std::floor(std::log2(resolution))) + 1;
  m_voxelRadiance = bufCtx->CreateImage(createInfo);

  m_voxelInfo.voxelSizeResolution.w = resolution;
  m_giInfo = bufCtx->CreateBuffer(UNIFORM_BUFFER, &m_voxelInfo, sizeof(VoxelInfo), false);

  m_setForVoxelization = pipelineCtx->CreateDescriptorSet(GetVoxelizationDescriptorArgument());
  // m_setForStaticVoxelization = pipelineCtx->CreateDescriptorSet(GetVoxelizationDescriptorArgument());
  m_setForLightInject = pipelineCtx->CreateDescriptorSet(GetLightInjectDescriptorArgument());

  // create image view
  m_voxelDiffuseView = bufCtx->CreateImageView(ImageViewCreateInfo{
      .image = m_voxelDiffuse,
      .type = ImageViewType::TEXTURE3D,
      .baseLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  });
  m_voxelNormalView = bufCtx->CreateImageView(ImageViewCreateInfo{
      .image = m_voxelNormal,
      .type = ImageViewType::TEXTURE3D,
      .baseLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  });
  // m_voxelStaticDiffuseView = bufCtx->CreateImageView(ImageViewCreateInfo{
  //     .image = m_voxelStaticDiffuse,
  //     .type = ImageViewType::TEXTURE3D,
  //     .format = ImageFormat::R32UI,
  //     .baseLevel = 0,
  //     .levelCount = 1,
  //     .baseArrayLayer = 0,
  //     .layerCount = 1,
  // });
  // m_voxelStaticNormalView = bufCtx->CreateImageView(ImageViewCreateInfo{
  //     .image = m_voxelStaticNormal,
  //     .type = ImageViewType::TEXTURE3D,
  //     .format = ImageFormat::R32UI,
  //     .baseLevel = 0,
  //     .levelCount = 1,
  //     .baseArrayLayer = 0,
  //     .layerCount = 1,
  // });
  m_diffuseVoxelizationView = bufCtx->CreateImageView(ImageViewCreateInfo{
      .image = m_voxelDiffuse,
      .type = ImageViewType::TEXTURE3D,
      .format = ImageFormat::R32UI,
      .baseLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  });
  m_normalVoxelizationView = bufCtx->CreateImageView(ImageViewCreateInfo{
      .image = m_voxelNormal,
      .type = ImageViewType::TEXTURE3D,
      .format = ImageFormat::R32UI,
      .baseLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  });
  m_voxelRadianceView = bufCtx->CreateImageView(ImageViewCreateInfo{
      .image = m_voxelRadiance,
      .type = ImageViewType::TEXTURE3D,
      .baseLevel = 0,
      .levelCount = static_cast<uint32_t>(std::floor(std::log2(resolution))) + 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  });

  // create sampler
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

  samplerCreateInfo.maxLod = static_cast<uint32_t>(std::floor(std::log2(resolution))) + 1,
  m_radianceSampler = pipelineCtx->CreateSampler(samplerCreateInfo);

  BindStaticVoxelizationSet();
  BindVoxelizationSet();
  BindLightInjectSet();
}

VoxelRenderComponent::~VoxelRenderComponent() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  bufCtx->DestroyImage(m_voxelDiffuse);
  bufCtx->DestroyImage(m_voxelNormal);
  bufCtx->DestroyImage(m_voxelRadiance);
  // bufCtx->DestroyImage(m_voxelStaticDiffuse);
  // bufCtx->DestroyImage(m_voxelStaticNormal);

  // destroy image view
  // bufCtx->DestroyImageView(m_voxelStaticDiffuseView);
  // bufCtx->DestroyImageView(m_voxelStaticNormalView);
  bufCtx->DestroyImageView(m_voxelDiffuseView);
  bufCtx->DestroyImageView(m_voxelNormalView);
  bufCtx->DestroyImageView(m_voxelRadianceView);

  pipelineCtx->DestroySampler(m_sampler);
  pipelineCtx->DestroySampler(m_radianceSampler);
}

DescriptorSetArgument&
VoxelRenderComponent::GetVoxelizationDescriptorArgument() {
  static DescriptorSetArgument argument({
      {0, DescriptorType::STORAGE_IMAGE},
      {1, DescriptorType::STORAGE_IMAGE},
      {2, DescriptorType::UNIFORM_BUFFER},
  });
  return argument;
}

DescriptorSetArgument&
VoxelRenderComponent::GetLightInjectDescriptorArgument() {
  static DescriptorSetArgument argument({
      {0, DescriptorType::IMAGE},
      {1, DescriptorType::IMAGE},
      {2, DescriptorType::STORAGE_IMAGE},
      {3, DescriptorType::UNIFORM_BUFFER},
  });
  return argument;
}

void
VoxelRenderComponent::BindVoxelizationSet() {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  pipelineCtx->BindStorageImage(BindStorageImageInfo{
      .descriptorSet = m_setForVoxelization,
      .bindingPoint = 0,
      .imageView = m_diffuseVoxelizationView,
  });
  pipelineCtx->BindStorageImage(BindStorageImageInfo{
      .descriptorSet = m_setForVoxelization,
      .bindingPoint = 1,
      .imageView = m_normalVoxelizationView,
  });
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_setForVoxelization,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 2,
      .buffer = m_giInfo,
      .offset = 0,
      .arrayElement = 0,
  });
}

void
VoxelRenderComponent::BindStaticVoxelizationSet() {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  // pipelineCtx->BindStorageImage(BindStorageImageInfo{
  //     .descriptorSet = m_setForStaticVoxelization,
  //     .bindingPoint = 0,
  //     .imageView = m_voxelStaticDiffuseView,
  // });
  // pipelineCtx->BindStorageImage(BindStorageImageInfo{
  //     .descriptorSet = m_setForStaticVoxelization,
  //     .bindingPoint = 1,
  //     .imageView = m_voxelStaticNormalView,
  // });
  // pipelineCtx->BindBuffer(BindBufferInfo{
  //     .descriptorSet = m_setForStaticVoxelization,
  //     .descriptorType = DescriptorType::UNIFORM_BUFFER,
  //     .bindingPoint = 2,
  //     .buffer = m_giInfo,
  //     .offset = 0,
  //     .arrayElement = 0,
  // });
}

void
VoxelRenderComponent::BindLightInjectSet() {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  pipelineCtx->BindImage(BindImageInfo{
      .descriptorSet = m_setForLightInject,
      .bindingPoint = 0,
      .imageView = m_voxelDiffuseView,
      .sampler = m_sampler,
  });
  pipelineCtx->BindImage(BindImageInfo{
      .descriptorSet = m_setForLightInject,
      .bindingPoint = 1,
      .imageView = m_voxelNormalView,
      .sampler = m_sampler,
  });
  pipelineCtx->BindStorageImage(BindStorageImageInfo{
      .descriptorSet = m_setForLightInject,
      .bindingPoint = 2,
      .imageView = m_voxelRadianceView,
  });
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_setForLightInject,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 3,
      .buffer = m_giInfo,
      .offset = 0,
      .arrayElement = 0,
  });
}

void
VoxelRenderComponent::UpdateVoxelInfo(const glm::vec3& size, const glm::vec3& pos) {
  m_voxelInfo.voxelSizeResolution.x = size.x / m_voxelInfo.voxelSizeResolution.w;
  m_voxelInfo.voxelSizeResolution.y = size.y / m_voxelInfo.voxelSizeResolution.w;
  m_voxelInfo.voxelSizeResolution.z = size.z / m_voxelInfo.voxelSizeResolution.w;

  auto viewX = glm::lookAt(pos + glm::vec3(size.x / 2, 0, 0), pos, glm::vec3(0, 1, 0));
  auto viewY = glm::lookAt(pos + glm::vec3(0, size.y / 2, 0), pos, glm::vec3(0, 0, -1));
  auto viewZ = glm::lookAt(pos + glm::vec3(0, 0, size.z / 2), pos, glm::vec3(0, 1, 0));

  auto projectMatrixX = glm::ortho(-size.z / 2, size.z / 2, -size.y / 2, size.y / 2, 0.f, size.x);
  auto projectMatrixY = glm::ortho(-size.x / 2, size.x / 2, -size.z / 2, size.z / 2, 0.f, size.y);
  auto projectMatrixZ = glm::ortho(-size.x / 2, size.x / 2, -size.y / 2, size.y / 2, 0.f, size.z);
  projectMatrixX[1][1] *= -1;
  projectMatrixY[1][1] *= -1;
  projectMatrixZ[1][1] *= -1;
  m_voxelInfo.projX = projectMatrixX * viewX;
  m_voxelInfo.projY = projectMatrixY * viewY;
  m_voxelInfo.projZ = projectMatrixZ * viewZ;

  auto bufCtx = m_rhiFactory->GetBufferContext();
  bufCtx->UpdateBuffer(m_giInfo, &m_voxelInfo, sizeof(VoxelInfo), 0);
}
};  // namespace Marbas
