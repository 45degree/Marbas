#include "ModelGPUData.hpp"

// #include "AssetManager/AssetManager.hpp"
#include "AssetManager/ModelAsset.hpp"
#include "Core/Scene/GPUDataPipeline/TextureGPUData.hpp"

namespace Marbas {

Image* MeshGPUData::m_emptyImage;
ImageView* MeshGPUData::m_emptyImageView;
static std::once_flag s_onceFlags;

MeshGPUData::MeshGPUData(const Mesh& mesh, RHIFactory* rhiFactory)
    : m_rhiFactory(rhiFactory), m_indexCount(mesh.m_indices.size()) {
  auto bufCtx = rhiFactory->GetBufferContext();
  auto pipelineCtx = rhiFactory->GetPipelineContext();
  auto vertexBufferSize = sizeof(Vertex) * mesh.m_vertices.size();
  auto vertexBufferData = mesh.m_vertices.data();
  auto indexBufferSize = sizeof(uint32_t) * mesh.m_indices.size();
  auto indexBufferData = mesh.m_indices.data();
  m_vertexBuffer = bufCtx->CreateBuffer(BufferType::VERTEX_BUFFER, vertexBufferData, vertexBufferSize, false);
  m_indexBuffer = bufCtx->CreateBuffer(BufferType::INDEX_BUFFER, indexBufferData, indexBufferSize, false);

  m_materialInfoBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_materialInfo, sizeof(MaterialInfo), false);
  m_transformBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, nullptr, sizeof(glm::mat4), false);

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

  std::call_once(s_onceFlags, [&]() {
    m_emptyImage = bufCtx->CreateImage(ImageCreateInfo{
        .width = 16,
        .height = 16,
        .format = ImageFormat::RGBA,
        .sampleCount = SampleCount::BIT1,
        .mipMapLevel = 1,
        .usage = ImageUsageFlags::SHADER_READ,
        .imageDesc = Image2DDesc(),
    });
    m_emptyImageView = bufCtx->CreateImageView(ImageViewCreateInfo{
        .image = m_emptyImage,
        .type = ImageViewType::TEXTURE2D,
        .baseLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    });
  });

  CreateAndBindDescriptorSet();
  Update(mesh);
};

MeshGPUData::~MeshGPUData() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  bufCtx->DestroyBuffer(m_vertexBuffer);
  bufCtx->DestroyBuffer(m_indexBuffer);
  pipelineCtx->DestroySampler(m_sampler);
  pipelineCtx->DestroyDescriptorSet(m_descriptorSet);
}

const DescriptorSetArgument&
MeshGPUData::GetDescriptorSetArgument() {
  static DescriptorSetArgument argument;
  static std::once_flag flags;
  std::call_once(flags, [&]() {
    argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
    argument.Bind(1, DescriptorType::UNIFORM_BUFFER);
    argument.Bind(2, DescriptorType::IMAGE);  // diffuse
    argument.Bind(3, DescriptorType::IMAGE);  // normal
    argument.Bind(4, DescriptorType::IMAGE);  // roughness
    argument.Bind(5, DescriptorType::IMAGE);  // metallic
  });

  return argument;
}

void
MeshGPUData::CreateAndBindDescriptorSet() {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  const auto& argument = GetDescriptorSetArgument();
  m_descriptorSet = pipelineCtx->CreateDescriptorSet(argument);
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_descriptorSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_materialInfoBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_descriptorSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 1,
      .buffer = m_transformBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

void
MeshGPUData::UpdateMaterial(const Mesh& mesh) {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto textureManager = AssetManager<TextureAsset>::GetInstance();
  auto textureGPUManager = TextureGPUDataManager::GetInstance();

  auto SetTexture = [&](const AssetPath& path) {
    if (!textureManager->Existed(path)) {
      textureManager->Create(path);
    }
    auto& asset = *textureManager->Get(path);
    if (!textureGPUManager->Existed(asset)) {
      textureGPUManager->Create(asset);
    }
    return textureGPUManager->TryGet(asset);
  };

  auto bindImage = [&](auto& textureAsset, uint16_t bindingPoint) {
    auto imageView = textureAsset->GetImageView();
    pipelineCtx->BindImage(BindImageInfo{
        .descriptorSet = m_descriptorSet,
        .bindingPoint = bindingPoint,
        .imageView = imageView,
        .sampler = m_sampler,
    });
  };

  auto bindEmptyImage = [&](uint16_t bindingPoint) {
    pipelineCtx->BindImage(BindImageInfo{
        .descriptorSet = m_descriptorSet,
        .bindingPoint = bindingPoint,
        .imageView = m_emptyImageView,
        .sampler = m_sampler,
    });
  };

  // update descriptor set binding
  if (auto bindingPoint = 2; mesh.m_material.m_diffuseTexturePath.has_value()) {
    m_diffuseTexture = SetTexture(*mesh.m_material.m_diffuseTexturePath);
    bindImage(m_diffuseTexture, bindingPoint);
  } else {
    bindEmptyImage(bindingPoint);
  }

  if (auto bindingPoint = 3; mesh.m_material.m_normalTexturePath.has_value()) {
    m_normalTexture = SetTexture(*mesh.m_material.m_normalTexturePath);
    bindImage(m_normalTexture, bindingPoint);
  } else {
    bindEmptyImage(bindingPoint);
  }

  if (auto bindingPoint = 4; mesh.m_material.m_roughnessTexturePath.has_value()) {
    m_roughnessTexture = SetTexture(*mesh.m_material.m_roughnessTexturePath);
    bindImage(m_roughnessTexture, bindingPoint);
  } else {
    bindEmptyImage(bindingPoint);
  }

  if (auto bindingPoint = 5; mesh.m_material.m_metalnessTexturePath.has_value()) {
    m_metallicTexture = SetTexture(*mesh.m_material.m_metalnessTexturePath);
    bindImage(m_metallicTexture, bindingPoint);
  } else {
    bindEmptyImage(bindingPoint);
  }
}

void
MeshGPUData::UpdateMaterialInfo(const Mesh& mesh) {
  m_materialInfo.texInfo.x = mesh.m_material.m_useDiffuseTexture;
  m_materialInfo.texInfo.y = mesh.m_material.m_useNormalTexture;
  m_materialInfo.texInfo.z = mesh.m_material.m_useMetalnessTexture;
  m_materialInfo.texInfo.w = mesh.m_material.m_useRoughnessTexture;

  const auto& color = mesh.m_material.m_diffuseColor;
  m_materialInfo.diffuseColor = glm::vec4(color[0], color[1], color[2], color[3]);
  m_materialInfo.metallicValue = mesh.m_material.m_metalnessValue;
  m_materialInfo.roughnessValue = mesh.m_material.m_roughnessValue;
}

void
MeshGPUData::Update(const Mesh& mesh) {
  auto bufCtx = m_rhiFactory->GetBufferContext();

  if (mesh.m_materialValueChanged) {
    UpdateMaterialInfo(mesh);
    bufCtx->UpdateBuffer(m_materialInfoBuffer, &m_materialInfo, sizeof(MaterialInfo), false);
  }

  if (mesh.m_materialTexChanged) {
    UpdateMaterial(mesh);
    UpdateMaterialInfo(mesh);
    bufCtx->UpdateBuffer(m_materialInfoBuffer, &m_materialInfo, sizeof(MaterialInfo), false);
  }
}

Task<>
ModelGPUData::Load(const Asset& asset) {
  auto meshCount = asset.GetMeshCount();

  for (int i = 0; i < meshCount; i++) {
    auto& mesh = asset.GetMesh(i);
    m_meshGPU.push_back(std::make_unique<MeshGPUData>(mesh, m_rhiFactory));
  }

  co_return;
}

Task<>
ModelGPUData::Update(const Asset& asset) {
  auto meshCount = asset.GetMeshCount();
  for (int i = 0; i < meshCount; i++) {
    auto& mesh = asset.GetMesh(i);
    m_meshGPU[i]->Update(mesh);
  }

  co_return;
}

}  // namespace Marbas
