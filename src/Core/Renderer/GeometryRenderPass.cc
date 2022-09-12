#include "Core/Renderer/GeometryRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Renderer/BeginningRenderPass.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "RHI/Interface/DescriptorSet.hpp"

namespace Marbas {

const String GeometryRenderPass::depthTargetName = "GeometryDepthTexture";
const String GeometryRenderPass::geometryTargetName = "GeometryColorNormalTexture";
const String GeometryRenderPass::renderPassName = "GeometryRenderPass";

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
                  .format = TextureFormat::RGBA,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
              AttachmentDescription{
                  .format = TextureFormat::RGB32F,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Clear,
              },
              AttachmentDescription{
                  .format = TextureFormat::RGB32F,
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
      .type = BufferDescriptorType::DYNAMIC_UNIFORM_BUFFER,
      .bindingPoint = 1,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = false,
      .bindingPoint = 0,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = false,
      .bindingPoint = 1,
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
  const auto& targetGBuffer = m_outputTarget[geometryTargetName]->GetGBuffer();
  auto normalBuffer = targetGBuffer->GetTexture(GBufferTexutreType::NORMALS);
  auto colorBuffer = targetGBuffer->GetTexture(GBufferTexutreType::COLOR);
  auto positionBuffer = targetGBuffer->GetTexture(GBufferTexutreType::POSITION);

  const auto& depthGBuffer = m_outputTarget[depthTargetName]->GetGBuffer();
  auto depthBuffer = depthGBuffer->GetTexture(GBufferTexutreType::DEPTH);

  if (normalBuffer == nullptr || colorBuffer == nullptr) {
    LOG(ERROR) << "can't get normal buffer or color buffer from the gbuffer";
    throw std::runtime_error("normal buffer and color buffer is needed by geometry render pass");
  }

  auto width = normalBuffer->GetWidth();
  auto height = normalBuffer->GetHeight();

  std::shared_ptr colorBufferView = m_rhiFactory->CreateImageView();
  colorBufferView->SetTexture(colorBuffer);
  std::shared_ptr normalBufferView = m_rhiFactory->CreateImageView();
  normalBufferView->SetTexture(normalBuffer);
  std::shared_ptr positionBufferView = m_rhiFactory->CreateImageView();
  positionBufferView->SetTexture(positionBuffer);
  std::shared_ptr depthBufferView = m_rhiFactory->CreateImageView();
  depthBufferView->SetTexture(depthBuffer);

  FrameBufferInfo createInfo{
      .width = width,
      .height = height,
      .renderPass = m_renderPass.get(),
      .attachments = {colorBufferView, normalBufferView, positionBufferView, depthBufferView},
  };

  m_framebuffer = m_rhiFactory->CreateFrameBuffer(createInfo);
}

void
GeometryRenderPass::RecordCommand(const Scene* scene) {
  m_commandBuffer->Clear();

  auto view = Entity::GetAllEntity<MeshComponent>(scene);

  // check framebuffer and renderpass
  DLOG_ASSERT(m_framebuffer != nullptr);
  DLOG_ASSERT(m_renderPass != nullptr);
  DLOG_ASSERT(m_pipeline != nullptr);

  // recreate dynamic uniform buffer
  auto bufferSize = view.size() * sizeof(MeshComponent::UniformBufferBlockData);
  m_dynamicUniforBuffer = m_rhiFactory->CreateDynamicUniforBuffer(bufferSize);

  /**
   * set command
   */
  // get input target GBuffer
  const auto& inputTargetGBuffer = m_inputTarget[BeginningRenderPass::targetName]->GetGBuffer();
  auto inputDepthGBuffer = m_inputTarget[BeginningRenderPass::depthTargetName]->GetGBuffer();
  auto inputColorTexture = inputTargetGBuffer->GetTexture(GBufferTexutreType::COLOR);
  auto inputdepthTexture = inputDepthGBuffer->GetTexture(GBufferTexutreType::DEPTH);

  const auto& outputTargetGBuffer = m_outputTarget[geometryTargetName]->GetGBuffer();
  auto outputColorBuffer = outputTargetGBuffer->GetTexture(GBufferTexutreType::COLOR);
  const auto& outputDepthGBuffer = m_outputTarget[depthTargetName]->GetGBuffer();
  auto outputDepthBuffer = outputDepthGBuffer->GetTexture(GBufferTexutreType::DEPTH);

  /**
   * begin to record command
   */
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

  // for every mesh entity, create bindVertexBuffer and bindIndexBuffer command and draw it
  uint32_t meshIndex = 0;
  for (auto&& [entity, meshComponent] : view.each()) {
    const auto& implData = meshComponent.m_impldata;
    implData->descriptorSet->BindDynamicBuffer(1, m_dynamicUniforBuffer);
    if (implData->vertexBuffer != nullptr && implData->indexBuffer != nullptr) {
      BindDescriptorSetInfo bindDescriptorSetInfo{
          .descriptorSet = implData->descriptorSet,
          .layouts = m_descriptorSetLayout,
      };
      if (implData->materialResource != nullptr) {
        auto size = sizeof(MeshComponent::UniformBufferBlockData);
        auto offset = sizeof(MeshComponent::UniformBufferBlockData) * meshIndex;
        bindDescriptorSetInfo.bufferPiece = {
            DynamicBufferPiece{
                .offset = static_cast<uint32_t>(offset),
                .size = static_cast<uint32_t>(size),
            },
        };
      }

      m_commandBuffer->BindDescriptorSet(std::move(bindDescriptorSetInfo));
      m_commandBuffer->BindVertexBuffer(implData->vertexBuffer);
      m_commandBuffer->BindIndexBuffer(implData->indexBuffer);
      m_commandBuffer->DrawIndex(implData->indexBuffer->GetIndexCount(), 0);

      meshIndex++;
    }
  }
  m_commandBuffer->EndRenderPass();
  m_commandBuffer->EndRecordCmd();
}

void
GeometryRenderPass::CreateBufferForEveryEntity(const MeshEntity& mesh, Scene* scene) {
  if (!Entity::HasComponent<MeshComponent>(scene, mesh)) return;

  auto& meshComponent = Entity::GetComponent<MeshComponent>(scene, mesh);
  if (meshComponent.m_impldata != nullptr) return;

  auto implData = std::make_shared<MeshComponent_Impl>();

  auto _mesh = meshComponent.m_mesh;

  // create vertex buffer and index buffer
  const auto& vertices = _mesh->m_vertices;
  auto verticesLen = sizeof(Vertex) * vertices.size();
  auto vertexBuffer = m_rhiFactory->CreateVertexBuffer(vertices.data(), verticesLen);
  vertexBuffer->SetLayout(GetMeshVertexInfoLayout());
  implData->vertexBuffer = std::move(vertexBuffer);

  const auto& indices = _mesh->m_indices;
  auto indexBuffer = m_rhiFactory->CreateIndexBuffer(indices);
  implData->indexBuffer = std::move(indexBuffer);

  // create descriptor Set
  implData->descriptorSet = m_rhiFactory->CreateDescriptorSet(m_descriptorSetLayout);
  implData->descriptorSet->BindBuffer(0, m_cameraUniformBuffer);

  // load material
  if (_mesh->m_materialId.has_value()) {
    auto id = _mesh->m_materialId.value();
    auto materialResource = m_resourceManager->GetMaterialResourceContainer()->GetResource(id);
    materialResource->LoadResource(m_rhiFactory, m_resourceManager.get());

    auto diffuseTexture = materialResource->GetDiffuseTexture();
    auto ambientTexture = materialResource->GetAmbientTexture();
    if (diffuseTexture != nullptr) {
      implData->descriptorSet->BindImage(0, diffuseTexture);
    }
    if (ambientTexture != nullptr) {
      implData->descriptorSet->BindImage(1, ambientTexture);
    }
    implData->materialResource = materialResource;
  }
  meshComponent.m_impldata = implData;
}

void
GeometryRenderPass::SetUniformBuffer(const Scene* scene) {
  // get matrix
  const auto editorCamera = scene->GetEditorCamrea();
  UpdateCameraUniformBuffer(editorCamera.get());

  uint32_t index = 0;
  auto view = Entity::GetAllEntity<MeshComponent>(const_cast<Scene*>(scene));
  for (const auto& [entity, meshComponent] : view.each()) {
    if (meshComponent.m_impldata == nullptr) continue;
    auto offset = index * sizeof(MeshComponent::UniformBufferBlockData);
    auto size = sizeof(MeshComponent::UniformBufferBlockData);

    if (!meshComponent.m_model.expired()) {
      const auto model = meshComponent.m_model.lock();
      const auto& modelMatrix = model->GetModelMatrix();
      meshComponent.m_uniformBufferData.model = modelMatrix;
    }

    m_dynamicUniforBuffer->SetData(&meshComponent.m_uniformBufferData, size, offset);
    index++;
  }
}

void
GeometryRenderPass::Execute(const Scene* scene, const ResourceManager* resourceManager) {
  auto view = Entity::GetAllEntity<MeshComponent>(scene);
  for (auto&& [entity, meshComponent] : view.each()) {
    CreateBufferForEveryEntity(entity, const_cast<Scene*>(scene));
  }

  auto bufferSize = view.size() * sizeof(MeshComponent::UniformBufferBlockData);
  if (m_dynamicUniforBuffer == nullptr || bufferSize != m_dynamicUniforBuffer->GetSize()) {
    RecordCommand(scene);
  }

  SetUniformBuffer(scene);
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
