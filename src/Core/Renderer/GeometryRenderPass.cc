#include "Core/Renderer/GeometryRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Renderer/BeginningRenderPass.hpp"
#include "Core/Scene/Component/MeshComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"

namespace Marbas {

const String GeometryRenderPass::depthTargetName = "GeometryDepthTexture";
const String GeometryRenderPass::geometryTargetName = "GeometryColorNormalTexture";
const String GeometryRenderPass::renderPassName = "GeometryRenderPass";

struct MeshComponent_Impl {
  std::shared_ptr<VertexBuffer> vertexBuffer;
  std::shared_ptr<IndexBuffer> indexBuffer;
  std::shared_ptr<DescriptorSet> descriptor;  // store the texture
  std::shared_ptr<MaterialResource> materialResource;
};

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

GeometryRenderPassCreatInfo::GeometryRenderPassCreatInfo() : DeferredRenderPassNodeCreateInfo() {
  passName = GeometryRenderPass::renderPassName;
  inputResource = {BeginningRenderPass::depthTargetName, BeginningRenderPass::targetName};
  outputResource = {GeometryRenderPass::depthTargetName, GeometryRenderPass::geometryTargetName};
}

GeometryRenderPass::GeometryRenderPass(const GeometryRenderPassCreatInfo& createInfo)
    : DeferredRenderPass(createInfo) {
  DLOG_ASSERT(m_rhiFactory != nullptr)
      << "can't Initialize the geometryRenderPass, because the rhiFactory isn't been set";

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
                  .format = TextureFormat::RGB,
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

  // create command factory
  m_commandFactory = m_rhiFactory->CreateCommandFactory();
  m_commandBuffer = m_commandFactory->CreateCommandBuffer();

  // read shader
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/shader.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/shader.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);

  // create pipeline
  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetVertexBufferLayout(GetMeshVertexInfoLayout(), VertexInputRate::VERTEX);
  m_pipeline->SetShader(shaderResource->GetShader());
  m_pipeline->Create();

  // create desciriptorSet
  m_dynamicDescriptorSet = m_rhiFactory->CreateDynamicDescriptorSet({0});
}

void
GeometryRenderPass::CreateFrameBuffer() {
  const auto& targetGBuffer = m_outputTarget[geometryTargetName]->GetGBuffer();
  auto normalBuffer = targetGBuffer->GetTexture(GBufferTexutreType::NORMALS);
  auto colorBuffer = targetGBuffer->GetTexture(GBufferTexutreType::COLOR);

  const auto& depthGBuffer = m_outputTarget[depthTargetName]->GetGBuffer();
  auto depthBuffer = depthGBuffer->GetTexture(GBufferTexutreType::DEPTH);

  if (normalBuffer == nullptr || colorBuffer == nullptr) {
    LOG(ERROR) << "can't get normal buffer or color buffer from the gbuffer";
    throw std::runtime_error("normal buffer and color buffer is needed by geometry render pass");
  }

  auto width = normalBuffer->GetWidth();
  auto height = normalBuffer->GetHeight();

  FrameBufferInfo createInfo{
      .width = width,
      .height = height,
      .renderPass = m_renderPass.get(),
      .attachments = {colorBuffer, normalBuffer, depthBuffer},
  };

  m_framebuffer = m_rhiFactory->CreateFrameBuffer(createInfo);
}

void
GeometryRenderPass::RecordCommand(const Scene* scene) {
  m_commandBuffer->Clear();

  auto view = Entity::GetAllEntity<MeshComponent>(scene);

  // check framebuffer and renderpass
  DLOG_ASSERT(m_framebuffer != nullptr)
      << FORMAT("{}'s framebuffer is null, can't record command", NAMEOF_TYPE(GeometryRenderPass));

  DLOG_ASSERT(m_renderPass != nullptr)
      << FORMAT("{}'s render pass is null, can't record command", NAMEOF_TYPE(GeometryRenderPass));

  DLOG_ASSERT(m_pipeline != nullptr)
      << FORMAT("{}'s pipeline is null, can't record command", NAMEOF_TYPE(GeometryRenderPass));

  // recreate dynamic uniform buffer
  Vector<MeshComponent::UniformBufferBlockData> uniformBufferData;
  for (auto&& [entity, meshComponent] : view.each()) {
    const auto& implData = meshComponent.m_impldata;
    if (implData == nullptr) continue;
    uniformBufferData.push_back(meshComponent.m_uniformBufferData);
  }
  auto bufferSize = uniformBufferData.size() * sizeof(MeshComponent::UniformBufferBlockData);

  m_dynamicUniforBuffer = m_rhiFactory->CreateDynamicUniforBuffer(bufferSize);
  m_dynamicUniforBuffer->SetData(uniformBufferData.data(), bufferSize, 0);

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

  // set dynamic descriptor set
  auto dynamicBufferDescriptor = m_dynamicUniforBuffer->GetIDynamicBufferDescriptor();
  m_dynamicDescriptorSet->BindDynamicBuffer(0, std::move(dynamicBufferDescriptor));

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
  m_commandBuffer->AddCommand(std::move(beginRenderPass));
  m_commandBuffer->AddCommand(std::move(bindPipeline));

  auto copyColorImageCMD = m_commandFactory->CreateCopyImageToImageCMD();
  auto copyDepthImageCMD = m_commandFactory->CreateCopyImageToImageCMD();
  copyColorImageCMD->SetSrcImage(inputColorTexture);
  copyColorImageCMD->SetDstImage(outputColorBuffer);
  copyDepthImageCMD->SetSrcImage(inputdepthTexture);
  copyDepthImageCMD->SetDstImage(outputDepthBuffer);

  m_commandBuffer->AddCommand(std::move(copyColorImageCMD));
  m_commandBuffer->AddCommand(std::move(copyDepthImageCMD));

  // for every mesh entity, create bindVertexBuffer and bindIndexBuffer command and draw it
  uint32_t meshIndex = 0;
  for (auto&& [entity, meshComponent] : view.each()) {
    const auto& implData = meshComponent.m_impldata;
    if (implData->vertexBuffer != nullptr && implData->indexBuffer != nullptr) {
      auto bindVertexBuffer = m_commandFactory->CreateBindVertexBufferCMD();
      auto bindIndexBuffer = m_commandFactory->CreateBindIndexBufferCMD();
      auto bindDynamicBuffer = m_commandFactory->CreateBindDynamicDescriptorSetCMD();
      auto drawIndex = m_commandFactory->CreateDrawIndexCMD(m_pipeline);

      bindVertexBuffer->SetVertexBuffer(implData->vertexBuffer);
      bindIndexBuffer->SetIndexBuffer(implData->indexBuffer);
      bindDynamicBuffer->SetDescriptorSet(m_dynamicDescriptorSet);
      bindDynamicBuffer->SetOffset(sizeof(MeshComponent::UniformBufferBlockData) * meshIndex);
      bindDynamicBuffer->SetSize(sizeof(MeshComponent::UniformBufferBlockData));

      if (implData->materialResource != nullptr) {
        auto bindDescriptorSet = m_commandFactory->CreateBindDescriptorSetCMD();
        bindDescriptorSet->SetDescriptor(implData->descriptor);
        m_commandBuffer->AddCommand(std::move(bindDescriptorSet));
      }
      drawIndex->SetIndexCount(implData->indexBuffer->GetIndexCount());

      m_commandBuffer->AddCommand(std::move(bindVertexBuffer));
      m_commandBuffer->AddCommand(std::move(bindIndexBuffer));
      m_commandBuffer->AddCommand(std::move(bindDynamicBuffer));
      m_commandBuffer->AddCommand(std::move(drawIndex));

      meshIndex++;
    }
  }
  m_commandBuffer->AddCommand(std::move(endRenderPass));
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
  implData->vertexBuffer = vertexBuffer;

  const auto& indices = _mesh->m_indices;
  auto indexBuffer = m_rhiFactory->CreateIndexBuffer(indices);
  implData->indexBuffer = indexBuffer;

  // load material
  if (_mesh->m_materialId.has_value()) {
    auto id = _mesh->m_materialId.value();
    auto materialResource = m_resourceManager->GetMaterialResourceContainer()->GetResource(id);
    materialResource->LoadResource(m_rhiFactory, m_resourceManager.get());

    DescriptorSetInfo descriptorSetInfo{
        DescriptorInfo{
            .isBuffer = false,
            .bindingPoint = 0,
        },
        DescriptorInfo{
            .isBuffer = false,
            .bindingPoint = 1,
        },
    };
    auto descriptor = m_rhiFactory->CreateDescriptorSet(descriptorSetInfo);
    auto diffuseTexture = materialResource->GetDiffuseTexture();
    auto ambientTexture = materialResource->GetAmbientTexture();
    if (diffuseTexture != nullptr) {
      descriptor->BindImage(0, diffuseTexture->GetDescriptor());
    }
    if (ambientTexture != nullptr) {
      descriptor->BindImage(1, ambientTexture->GetDescriptor());
    }

    implData->materialResource = materialResource;
    implData->descriptor = descriptor;
  }

  meshComponent.m_impldata = implData;

  m_needToRecordComand = true;
}

void
GeometryRenderPass::SetUniformBuffer(const Scene* scene) {
  // get matrix
  const auto editorCamera = scene->GetEditorCamrea();
  const auto viewMatrix = editorCamera->GetViewMartix();
  const auto perspectiveMatrix = editorCamera->GetPerspective();

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

    meshComponent.m_uniformBufferData.view = viewMatrix;
    meshComponent.m_uniformBufferData.projective = perspectiveMatrix;

    m_dynamicUniforBuffer->SetData(&meshComponent.m_uniformBufferData, size, offset);
    index++;
  }
}

void
GeometryRenderPass::Execute(const Scene* scene, const ResourceManager* resourceManager) {
  // CreateFrameBuffer();
  auto view = Entity::GetAllEntity<MeshComponent>(scene);
  for (auto&& [entity, meshComponent] : view.each()) {
    CreateBufferForEveryEntity(entity, const_cast<Scene*>(scene));
  }

  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }

  SetUniformBuffer(scene);
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
