#include "Core/Renderer/CubeMapRenderPass.hpp"

#include <glog/logging.h>

#include <nameof.hpp>

#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Scene/Component/CubeMapComponent.hpp"

namespace Marbas {

struct CubeMapComponent_Impl {
  std::shared_ptr<VertexBuffer> vertexBuffer;
  std::shared_ptr<IndexBuffer> indexBuffer;
  std::shared_ptr<DescriptorSet> descriptorSet;
  std::shared_ptr<TextureCubeMapResource> textureCubeMapResource;
};

// TODO: need to change
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

CubeMapRenderPassCreateInfo::CubeMapRenderPassCreateInfo() { passName = "CubeMapRenderPass"; }

CubeMapRenderPass::CubeMapRenderPass(const CubeMapRenderPassCreateInfo& createInfo)
    : ForwardRenderPass(createInfo) {
  DLOG_ASSERT(m_rhiFactory != nullptr)
      << FORMAT("can't Initialize the {}, because the rhiFactory isn't been set",
                NAMEOF_TYPE(CubeMapRenderPass));

  /**
   * set render pass and pipeline
   */

  // create render pass
  RenderPassCreateInfo renderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
                  // result attachment
                  .format = TextureFormat::RGBA,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
              AttachmentDescription{
                  // input depth attachment from geometryRenderPass's output
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
  auto fragmentShader = m_rhiFactory->CreateShaderStage(
      "Shader/cubeMap.frag.glsl", ShaderCodeType::FILE, ShaderType::FRAGMENT_SHADER);
  auto vertexShader = m_rhiFactory->CreateShaderStage(
      "Shader/cubeMap.vert.glsl", ShaderCodeType::FILE, ShaderType::VERTEX_SHADER);
  auto cubeMapShader = m_rhiFactory->CreateShader();
  cubeMapShader->AddShaderStage(vertexShader);
  cubeMapShader->AddShaderStage(fragmentShader);
  cubeMapShader->Link();

  // create Pipeline
  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetVertexBufferLayout(GetMeshVertexInfoLayout());
  m_pipeline->SetShader(cubeMapShader);
  m_pipeline->SetDepthStencilInfo(DepthStencilInfo{
      .depthTestEnable = true,
      .depthCompareOp = DepthCompareOp::LEQUAL,
  });
  m_pipeline->Create();

  // create desciriptorSet
  m_dynamicDescriptorSet = m_rhiFactory->CreateDynamicDescriptorSet({0});
}

void
CubeMapRenderPass::RecordCommand(const std::shared_ptr<Scene>& scene) {
  m_commandBuffer->Clear();

  auto view = Entity::GetAllEntity<CubeMapComponent>(scene);

  // check framebuffer and renderpass
  DLOG_ASSERT(view.size() == 1) << "cube map should only have one in the scene";
  DLOG_ASSERT(m_framebuffer != nullptr)
      << FORMAT("{}'s framebuffer is null, can't record command", NAMEOF_TYPE(CubeMapRenderPass));

  DLOG_ASSERT(m_renderPass != nullptr)
      << FORMAT("{}'s render pass is null, can't record command", NAMEOF_TYPE(CubeMapRenderPass));

  DLOG_ASSERT(m_pipeline != nullptr)
      << FORMAT("{}'s pipeline is null, can't record command", NAMEOF_TYPE(CubeMapRenderPass));

  // recreate dynamic uniform buffer
  Vector<CubeMapComponent::UniformBufferBlockData> uniformBufferData;
  for (auto&& [entity, cubeMapComponent] : view.each()) {
    const auto& implData = cubeMapComponent.m_implData;
    if (implData == nullptr) continue;
    uniformBufferData.push_back(cubeMapComponent.m_uniformBufferData);
  }
  auto bufferSize = uniformBufferData.size() * sizeof(CubeMapComponent::UniformBufferBlockData);

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

  /**
   * begin to record command
   */

  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->AddCommand(std::move(beginRenderPass));
  m_commandBuffer->AddCommand(std::move(bindPipeline));

  auto entity = view.front();
  auto& cubeMapComponent = Entity::GetComponent<CubeMapComponent>(scene, entity);
  const auto& implData = cubeMapComponent.m_implData;
  DLOG_ASSERT(implData->vertexBuffer != nullptr);
  DLOG_ASSERT(implData->indexBuffer != nullptr);

  auto bindVertexBuffer = m_commandFactory->CreateBindVertexBufferCMD();
  auto bindIndexBuffer = m_commandFactory->CreateBindIndexBufferCMD();
  auto bindDynamicBuffer = m_commandFactory->CreateBindDynamicDescriptorSetCMD();
  auto drawIndex = m_commandFactory->CreateDrawIndexCMD();

  bindVertexBuffer->SetVertexBuffer(implData->vertexBuffer);
  bindIndexBuffer->SetIndexBuffer(implData->indexBuffer);
  bindDynamicBuffer->SetDescriptorSet(m_dynamicDescriptorSet);
  bindDynamicBuffer->SetOffset(0);
  bindDynamicBuffer->SetSize(sizeof(CubeMapComponent::UniformBufferBlockData));

  if (implData->textureCubeMapResource != nullptr) {
    auto bindDescriptorSet = m_commandFactory->CreateBindDescriptorSetCMD();
    bindDescriptorSet->SetDescriptor(implData->descriptorSet);
    m_commandBuffer->AddCommand(std::move(bindDescriptorSet));
  }

  drawIndex->SetIndexCount(implData->indexBuffer->GetIndexCount());

  m_commandBuffer->AddCommand(std::move(bindVertexBuffer));
  m_commandBuffer->AddCommand(std::move(bindIndexBuffer));
  m_commandBuffer->AddCommand(std::move(bindDynamicBuffer));
  m_commandBuffer->AddCommand(std::move(drawIndex));

  m_commandBuffer->AddCommand(std::move(endRenderPass));
  m_commandBuffer->EndRecordCmd();
}

void
CubeMapRenderPass::CreateBufferForEveryEntity(const entt::entity cubeMap,
                                              const std::shared_ptr<Scene>& scene) {
  DLOG_ASSERT(Entity::HasComponent<CubeMapComponent>(scene, cubeMap));

  auto& cubeMapComponent = Entity::GetComponent<CubeMapComponent>(scene, cubeMap);
  if (cubeMapComponent.m_implData != nullptr) return;

  auto implData = std::make_shared<CubeMapComponent_Impl>();

  // create vertex buffer and index buffer
  const auto& vertices = CubeMapComponent::vertices;
  Vector<uint32_t> indices(CubeMapComponent::indices.begin(), CubeMapComponent::indices.end());

  auto verticesLen = sizeof(Vertex) * vertices.size();
  auto vertexBuffer = m_rhiFactory->CreateVertexBuffer(vertices.data(), verticesLen);
  vertexBuffer->SetLayout(GetMeshVertexInfoLayout());
  implData->vertexBuffer = vertexBuffer;

  auto indexBuffer = m_rhiFactory->CreateIndexBuffer(std::move(indices));
  implData->indexBuffer = indexBuffer;

  // load material
  if (cubeMapComponent.cubeMapResourceId.has_value()) {
    auto id = cubeMapComponent.cubeMapResourceId.value();
    auto cubeMapResource = m_resourceManager->GetCubeMapResourceContainer()->GetResource(id);
    cubeMapResource->LoadResource(m_rhiFactory, m_resourceManager);

    DescriptorSetInfo descriptorSetInfo{
        DescriptorInfo{
            .isBuffer = false,
            .bindingPoint = 0,
        },
    };
    auto descriptor = m_rhiFactory->CreateDescriptorSet(descriptorSetInfo);
    descriptor->BindImage(0, cubeMapResource->GetTextureCubeMap()->GetDescriptor());

    implData->descriptorSet = descriptor;
    implData->textureCubeMapResource = cubeMapResource;
  }
  cubeMapComponent.m_implData = implData;
  m_needToRecordComand = true;
}

void
CubeMapRenderPass::SetUniformBuffer(const std::shared_ptr<Scene>& scene) {
  // get matrix
  const auto editorCamera = scene->GetEditorCamrea();
  const auto viewMatrix = editorCamera->GetViewMartix();
  const auto perspectiveMatrix = editorCamera->GetPerspective();

  uint32_t index = 0;
  auto view = Entity::GetAllEntity<CubeMapComponent>(scene);
  for (const auto& [entity, cubeMapComponent] : view.each()) {
    if (cubeMapComponent.m_implData == nullptr) continue;
    auto offset = index * sizeof(MeshComponent::UniformBufferBlockData);
    auto size = sizeof(MeshComponent::UniformBufferBlockData);

    cubeMapComponent.m_uniformBufferData.model = glm::mat4(1);

    cubeMapComponent.m_uniformBufferData.view = glm::mat4(glm::mat3(viewMatrix));
    cubeMapComponent.m_uniformBufferData.projective = perspectiveMatrix;

    m_dynamicUniforBuffer->SetData(&cubeMapComponent.m_uniformBufferData, size, offset);
    index++;
  }
}

void
CubeMapRenderPass::Execute(const std::shared_ptr<Scene>& scene,
                           const std::shared_ptr<ResourceManager>& resourceMnager) {
  auto view = Entity::GetAllEntity<CubeMapComponent>(scene);
  for (auto&& [entity, cubeMapComponent] : view.each()) {
    CreateBufferForEveryEntity(entity, scene);
  }

  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }

  SetUniformBuffer(scene);
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
