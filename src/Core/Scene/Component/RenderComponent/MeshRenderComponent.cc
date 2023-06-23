#include "MeshRenderComponent.hpp"

#include <glog/logging.h>

namespace Marbas {

MeshRenderComponent::MeshRenderComponent(RHIFactory* rhiFactory, uintptr_t sampler, ImageView* emptyImageView)
    : m_rhiFactory(rhiFactory), m_externSampler(sampler), m_externEmptyImageView(emptyImageView) {}

MeshRenderComponent::~MeshRenderComponent() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  bufCtx->DestroyBuffer(m_materialInfoBuffer);
}

void
MeshRenderComponent::Init(Mesh& mesh) {
  CreateMeshVertexIndexData(mesh);
  CreateMaterialBuffer(mesh);
  CreateMeshDescriptorSet(mesh);
}

void
MeshRenderComponent::Update(Mesh& mesh) {
  if (mesh.m_materialTexChanged) {
    UpdateMaterial(mesh);
    UpdateMaterialInfo(mesh);
    return;
  } else if (mesh.m_materialValueChanged) {
    UpdateMaterialInfo(mesh);
    return;
  }
  return;
}

const DescriptorSetArgument&
MeshRenderComponent::GetDescriptorSetArgument() {
  static DescriptorSetArgument argument;
  static std::once_flag flags;
  std::call_once(flags, [&]() {
    argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
    argument.Bind(1, DescriptorType::IMAGE);  // diffuse
    argument.Bind(2, DescriptorType::IMAGE);  // normal
    argument.Bind(3, DescriptorType::IMAGE);  // roughness
    argument.Bind(4, DescriptorType::IMAGE);  // metallic
  });

  return argument;
}

void
MeshRenderComponent::CreateMeshDescriptorSet(const Mesh& mesh) {
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
}

void
MeshRenderComponent::CreateMeshVertexIndexData(const Mesh& mesh) {
  using enum BufferType;

  m_indexCount = mesh.m_indices.size();

  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto vertexBufferSize = sizeof(Vertex) * mesh.m_vertices.size();
  auto vertexBufferData = mesh.m_vertices.data();
  auto indexBufferSize = sizeof(uint32_t) * mesh.m_indices.size();
  auto indexBufferData = mesh.m_indices.data();
  m_vertexBuffer = bufCtx->CreateBuffer(VERTEX_BUFFER, vertexBufferData, vertexBufferSize, false);
  m_indexBuffer = bufCtx->CreateBuffer(INDEX_BUFFER, indexBufferData, indexBufferSize, false);
}

void
MeshRenderComponent::CreateMaterialBuffer(Mesh& mesh) {
  auto bufCtx = m_rhiFactory->GetBufferContext();

  constexpr auto materialSize = sizeof(MeshRenderComponent::MaterialInfo);
  auto& materialInfo = m_materialInfo;
  auto* materialBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &materialInfo, materialSize, false);
  m_materialInfoBuffer = materialBuffer;
}

void
MeshRenderComponent::UpdateMaterialInfo(const Mesh& mesh) {
  m_materialInfo.texInfo.x = mesh.m_material.m_useDiffuseTexture;
  m_materialInfo.texInfo.y = mesh.m_material.m_useNormalTexture;
  m_materialInfo.texInfo.z = mesh.m_material.m_useMetalnessTexture;
  m_materialInfo.texInfo.w = mesh.m_material.m_useRoughnessTexture;

  const auto& color = mesh.m_material.m_diffuseColor;
  m_materialInfo.diffuseColor = glm::vec4(color[0], color[1], color[2], color[3]);
  m_materialInfo.metallicValue = mesh.m_material.m_metalnessValue;
  m_materialInfo.roughnessValue = mesh.m_material.m_roughnessValue;

  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto bufferSize = sizeof(MeshRenderComponent::MaterialInfo);
  bufCtx->UpdateBuffer(m_materialInfoBuffer, &m_materialInfo, bufferSize, false);
}

void
MeshRenderComponent::UpdateMaterial(Mesh& mesh) {
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
        .sampler = m_externSampler,
    });
  };

  auto bindEmptyImage = [&](uint16_t bindingPoint) {
    pipelineCtx->BindImage(BindImageInfo{
        .descriptorSet = m_descriptorSet,
        .bindingPoint = bindingPoint,
        .imageView = m_externEmptyImageView,
        .sampler = m_externSampler,
    });
  };

  // update descriptor set binding
  if (auto bindingPoint = 1; mesh.m_material.m_diffuseTexturePath.has_value()) {
    try {
      m_diffuseTexture = SetTexture(*mesh.m_material.m_diffuseTexturePath);
      bindImage(m_diffuseTexture, bindingPoint);
    } catch (AssetException& e) {
      LOG(ERROR) << "cause an exception when load diffuse texture for model asset";
      LOG(ERROR) << "try to bind the empty image";
      LOG(ERROR) << e.what();
      bindEmptyImage(bindingPoint);
      mesh.m_material.m_diffuseTexturePath = std::nullopt;
      mesh.m_material.m_useDiffuseTexture = false;
    }
  } else {
    bindEmptyImage(bindingPoint);
  }

  if (auto bindingPoint = 2; mesh.m_material.m_normalTexturePath.has_value()) {
    try {
      m_normalTexture = SetTexture(*mesh.m_material.m_normalTexturePath);
      bindImage(m_normalTexture, bindingPoint);
    } catch (AssetException& e) {
      LOG(ERROR) << "cause an exception when load normal texture for model asset";
      LOG(ERROR) << "try to bind the empty image";
      LOG(ERROR) << e.what();
      bindEmptyImage(bindingPoint);
      mesh.m_material.m_normalTexturePath = std::nullopt;
      mesh.m_material.m_useNormalTexture = false;
    }
  } else {
    bindEmptyImage(bindingPoint);
  }

  if (auto bindingPoint = 3; mesh.m_material.m_roughnessTexturePath.has_value()) {
    try {
      m_roughnessTexture = SetTexture(*mesh.m_material.m_roughnessTexturePath);
      bindImage(m_roughnessTexture, bindingPoint);
    } catch (AssetException& e) {
      LOG(ERROR) << "cause an exception when load roughness texture for model asset";
      LOG(ERROR) << "try to bind the empty image";
      LOG(ERROR) << e.what();
      bindEmptyImage(bindingPoint);
      mesh.m_material.m_roughnessTexturePath = std::nullopt;
      mesh.m_material.m_useRoughnessTexture = false;
    }
  } else {
    bindEmptyImage(bindingPoint);
  }

  if (auto bindingPoint = 4; mesh.m_material.m_metalnessTexturePath.has_value()) {
    try {
      m_metallicTexture = SetTexture(*mesh.m_material.m_metalnessTexturePath);
      bindImage(m_metallicTexture, bindingPoint);
    } catch (AssetException& e) {
      LOG(ERROR) << "cause an exception when load metallic texture for model asset";
      LOG(ERROR) << "try to bind the empty image";
      LOG(ERROR) << e.what();
      bindEmptyImage(bindingPoint);
      mesh.m_material.m_metalnessTexturePath = std::nullopt;
      mesh.m_material.m_useMetalnessTexture = false;
    }
  } else {
    bindEmptyImage(bindingPoint);
  }
}

}  // namespace Marbas
