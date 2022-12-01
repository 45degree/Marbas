#include "Core/Renderer/GeometryRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Common.hpp"
#include "Core/Renderer/BeginningRenderPass.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "RHI/Interface/DescriptorSet.hpp"

namespace Marbas {

const String GeometryRenderPass::depthTargetName = "GeometryDepthTexture";
const String GeometryRenderPass::geometryTargetName = "GeometryColorNormalTexture";
const String GeometryRenderPass::renderPassName = "GeometryRenderPass";

GeometryRenderPassCreatInfo::GeometryRenderPassCreatInfo() : DeferredRenderPassCreateInfo() {
  passName = GeometryRenderPass::renderPassName;
  inputResource = {BeginningRenderPass::depthTargetName, BeginningRenderPass::targetName};
  outputResource = {GeometryRenderPass::depthTargetName, GeometryRenderPass::geometryTargetName};
}

GeometryRenderPass::GeometryRenderPass(const GeometryRenderPassCreatInfo& createInfo)
    : DeferredRenderPass(createInfo) {
  DLOG_ASSERT(m_rhiFactory != nullptr)
      << "can't Initialize the geometryRenderPass, because the rhiFactory isn't been set";
}

void
GeometryRenderPass::CreateRenderPass() {
  // create render pass
  RenderPassCreateInfo renderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
                  // color
                  .format = TextureFormat::RGBA,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
              AttachmentDescription{
                  // normal
                  .format = TextureFormat::RGB32F,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Clear,
              },
              AttachmentDescription{
                  // position
                  .format = TextureFormat::RGB32F,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Clear,
              },
              AttachmentDescription{
                  // Ambient Occlusion Texture
                  .format = TextureFormat::R32,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Clear,
              },
              AttachmentDescription{
                  // roughtness Texture
                  .format = TextureFormat::R32,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Clear,
              },
              AttachmentDescription{
                  // metaliic Texture
                  .format = TextureFormat::R32,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Clear,
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
GeometryRenderPass::CreateShader() {
  // read shader
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/geometry.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/geometry.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);
}

void
GeometryRenderPass::CreateDescriptorSetLayout() {
  // set descriptor set layout
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = true,
      .type = BufferDescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 1,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // albedo texture
      .isBuffer = false,
      .bindingPoint = 0,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // ambient occlusion texture
      .isBuffer = false,
      .bindingPoint = 1,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // normal texture
      .isBuffer = false,
      .bindingPoint = 2,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // roughness texture
      .isBuffer = false,
      .bindingPoint = 3,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // metallic texture
      .isBuffer = false,
      .bindingPoint = 4,
  });
}

void
GeometryRenderPass::CreatePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto resource = shaderContainer->GetResource(m_shaderId);
  if (!resource->IsLoad()) {
    resource->LoadResource(m_rhiFactory, m_resourceManager.get());
  }

  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetVertexBufferLayout(GetMeshVertexInfoLayout(), VertexInputRate::VERTEX);
  m_pipeline->SetPipelineLayout(GraphicsPipeLineLayout{
      .descriptorSetLayout = m_descriptorSetLayout,
  });
  m_pipeline->SetShader(resource->GetShader());
  m_pipeline->Create();
}

void
GeometryRenderPass::CreateFrameBuffer() {
  const auto& targetGBuffer = m_outputTarget[geometryTargetName];
  auto normalBuffer = targetGBuffer->GetGBuffer(GBufferTexutreType::NORMALS);
  auto colorBuffer = targetGBuffer->GetGBuffer(GBufferTexutreType::COLOR);
  auto positionBuffer = targetGBuffer->GetGBuffer(GBufferTexutreType::POSITION);
  auto ambientOcclusion = targetGBuffer->GetGBuffer(GBufferTexutreType::AMBIENT_OCCLUSION);
  auto roughness = targetGBuffer->GetGBuffer(GBufferTexutreType::ROUGHTNESS);
  auto metallic = targetGBuffer->GetGBuffer(GBufferTexutreType::METALLIC);

  const auto& depthGBuffer = m_outputTarget[depthTargetName];
  auto depthBuffer = depthGBuffer->GetGBuffer(GBufferTexutreType::DEPTH);

  if (normalBuffer == nullptr || colorBuffer == nullptr) {
    LOG(ERROR) << "can't get normal buffer or color buffer from the gbuffer";
    throw std::runtime_error("normal buffer and color buffer is needed by geometry render pass");
  }

  auto width = normalBuffer->GetWidth();
  auto height = normalBuffer->GetHeight();

  std::shared_ptr colorBufferView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = colorBuffer,
      .m_format = colorBuffer->GetFormat(),
      .m_type = colorBuffer->GetTextureType(),
  });
  std::shared_ptr normalBufferView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = normalBuffer,
      .m_format = normalBuffer->GetFormat(),
      .m_type = normalBuffer->GetTextureType(),
  });
  std::shared_ptr positionBufferView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = positionBuffer,
      .m_format = positionBuffer->GetFormat(),
      .m_type = positionBuffer->GetTextureType(),
  });
  std::shared_ptr depthBufferView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = depthBuffer,
      .m_format = depthBuffer->GetFormat(),
      .m_type = depthBuffer->GetTextureType(),
  });
  std::shared_ptr ambientOcclusionView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = ambientOcclusion,
      .m_format = ambientOcclusion->GetFormat(),
      .m_type = ambientOcclusion->GetTextureType(),
  });
  std::shared_ptr roughnessView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = roughness,
      .m_format = roughness->GetFormat(),
      .m_type = roughness->GetTextureType(),
  });
  std::shared_ptr metallicView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = metallic,
      .m_format = metallic->GetFormat(),
      .m_type = metallic->GetTextureType(),
  });

  FrameBufferInfo createInfo{
      .width = width,
      .height = height,
      .renderPass = m_renderPass.get(),
      .attachments =
          {
              colorBufferView,
              normalBufferView,
              positionBufferView,
              ambientOcclusionView,
              roughnessView,
              metallicView,
              depthBufferView,
          },
  };

  m_framebuffer = m_rhiFactory->CreateFrameBuffer(createInfo);
}

void
GeometryRenderPass::RecordCommand(const Scene* scene) {}

void
GeometryRenderPass::CreateBufferForEntity(const MeshEntity& mesh, Scene* scene) {
  if (!Entity::HasComponent<MeshComponent>(scene, mesh)) return;

  auto& meshComponent = Entity::GetComponent<MeshComponent>(scene, mesh);
  std::shared_ptr<MeshComponent_Impl> implData = meshComponent.m_impldata;
  if (implData == nullptr) {
    implData = std::make_shared<MeshComponent_Impl>();
    meshComponent.m_impldata = implData;
  }

  auto _mesh = meshComponent.m_mesh;
  if (!_mesh->m_needLoad) return;

  meshComponent.LoadMeshImpl(m_rhiFactory, m_descriptorSetLayout);
  implData->descriptorSet->BindBuffer(0, m_cameraUniformBuffer);

  // load material
  if (_mesh->m_materialId.has_value()) {
    auto id = _mesh->m_materialId.value();
    auto materialResource = m_resourceManager->GetMaterialResourceContainer()->GetResource(id);
    materialResource->LoadResource(m_rhiFactory, m_resourceManager.get());

    auto diffuseTexture = materialResource->GetAlbedoTexture();
    auto ambientTexture = materialResource->GetAmbientOcclusionTexture();
    auto roughtnessTexture = materialResource->GetRoughnessTexture();
    auto normalTexture = materialResource->GetNormalTexture();
    auto metallicTexture = materialResource->GetMetallicTexture();

    if (diffuseTexture != nullptr) {
      implData->descriptorSet->BindImage(0, diffuseTexture);
    }
    if (ambientTexture != nullptr) {
      implData->descriptorSet->BindImage(1, ambientTexture);
    }
    if (normalTexture != nullptr) {
      implData->descriptorSet->BindImage(2, normalTexture);
    }
    if (roughtnessTexture != nullptr) {
      implData->descriptorSet->BindImage(3, roughtnessTexture);
    }
    if (metallicTexture != nullptr) {
      implData->descriptorSet->BindImage(4, metallicTexture);
    }

    implData->materialResource = materialResource;
  }
  _mesh->m_needLoad = false;
}

void
GeometryRenderPass::SetUniformBufferForEntity(const Scene* scene, const MeshEntity& mesh) {
  auto materialContainer = m_resourceManager->GetMaterialResourceContainer();

  // TODO: remove
  const auto editorCamera = scene->GetEditorCamera();
  UpdateCameraUniformBuffer(editorCamera.get());

  auto size = sizeof(MeshComponent::UniformBufferBlockData);
  auto meshComponent = Entity::GetComponent<MeshComponent>(const_cast<Scene*>(scene), mesh);
  meshComponent.m_impldata->uniformBuffer->SetData(&meshComponent.m_uniformBufferData, size, 0);
}

void
GeometryRenderPass::RecordCommandForEntity(const Scene* scene, const entt::entity mesh) {
  const auto& hierarchyComponent = Entity::GetComponent<HierarchyComponent>(scene, mesh);
  const auto& meshComponent = Entity::GetComponent<MeshComponent>(scene, mesh);
  const auto& transformMAtrix = hierarchyComponent.globalTransformMatrix;
  const auto implData = meshComponent.m_impldata;

  implData->descriptorSet->BindBuffer(1, implData->uniformBuffer);

  BindDescriptorSetInfo bindDescriptorSetInfo{
      .descriptorSet = implData->descriptorSet,
      .layouts = m_descriptorSetLayout,
  };
  m_commandBuffer->BindDescriptorSet(std::move(bindDescriptorSetInfo));
  m_commandBuffer->BindVertexBuffer(implData->vertexBuffer);
  m_commandBuffer->BindIndexBuffer(implData->indexBuffer);
  m_commandBuffer->DrawIndex(implData->indexBuffer->GetIndexCount(), 0);
}

void
GeometryRenderPass::Execute(const Scene* scene, const ResourceManager* resourceManager) {
  auto camera = scene->GetEditorCamera();
  auto view = Entity::GetAllEntity<MeshComponent, HierarchyComponent>(scene);

  // get input target GBuffer
  const auto& inputTargetGBuffer = m_inputTarget[BeginningRenderPass::targetName];
  auto inputDepthGBuffer = m_inputTarget[BeginningRenderPass::depthTargetName];
  auto inputColorTexture = inputTargetGBuffer->GetGBuffer(GBufferTexutreType::COLOR);
  auto inputdepthTexture = inputDepthGBuffer->GetGBuffer(GBufferTexutreType::DEPTH);

  const auto& outputTargetGBuffer = m_outputTarget[geometryTargetName];
  auto outputColorBuffer = outputTargetGBuffer->GetGBuffer(GBufferTexutreType::COLOR);
  const auto& outputDepthGBuffer = m_outputTarget[depthTargetName];
  auto outputDepthBuffer = outputDepthGBuffer->GetGBuffer(GBufferTexutreType::DEPTH);

  m_commandBuffer->Clear();
  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->BeginRenderPass(BeginRenderPassInfo{
      .renderPass = m_renderPass,
      .frameBuffer = m_framebuffer,
      .clearColor = {0, 0, 0, 1},
  });
  m_commandBuffer->BindPipeline(m_pipeline);
  m_commandBuffer->CopyImageToImage(CopyImageToImageInfo{
      .srcTexture = inputColorTexture,
      .dstTexture = outputColorBuffer,
  });
  m_commandBuffer->CopyImageToImage(CopyImageToImageInfo{
      .srcTexture = inputdepthTexture,
      .dstTexture = outputDepthBuffer,
  });

  for (auto&& [entity, meshComponent, hierarchyComponent] : view.each()) {
    if (camera->IsMeshVisible(*meshComponent.m_mesh, hierarchyComponent.globalTransformMatrix)) {
      CreateBufferForEntity(entity, const_cast<Scene*>(scene));
      SetUniformBufferForEntity(scene, entity);
      RecordCommandForEntity(scene, entity);
    }
  }
  m_commandBuffer->EndRenderPass();
  m_commandBuffer->EndRecordCmd();

  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
