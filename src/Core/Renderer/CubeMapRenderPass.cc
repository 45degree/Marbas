#include "Core/Renderer/CubeMapRenderPass.hpp"

#include <glog/logging.h>

#include <nameof.hpp>

#include "Core/Common.hpp"
#include "Core/Renderer/BlinnPhongRenderPass.hpp"
#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Scene/Component/CubeMapComponent.hpp"

namespace Marbas {

CubeMapRenderPassCreateInfo::CubeMapRenderPassCreateInfo() {
  passName = "CubeMapRenderPass";
  inputPassNode = BlinnPhongRenderPass::renderPassName;
}

CubeMapRenderPass::CubeMapRenderPass(const CubeMapRenderPassCreateInfo& createInfo)
    : ForwardRenderPass(createInfo) {}

void
CubeMapRenderPass::CreateRenderPass() {
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
}

void
CubeMapRenderPass::CreateDescriptorSetLayout() {
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = false,
      .bindingPoint = 0,
  });
}

void
CubeMapRenderPass::CreateShader() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/cubeMap.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/cubeMap.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);
}

void
CubeMapRenderPass::CreatePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto resource = shaderContainer->GetResource(m_shaderId);
  if (!resource->IsLoad()) {
    resource->LoadResource(m_rhiFactory, m_resourceManager.get());
  }

  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetVertexBufferLayout(GetMeshVertexInfoLayout(), VertexInputRate::VERTEX);
  m_pipeline->SetShader(resource->GetShader());
  m_pipeline->SetDepthStencilInfo(DepthStencilInfo{
      .depthTestEnable = true,
      .depthCompareOp = DepthCompareOp::LEQUAL,
  });
  m_pipeline->Create();
}

void
CubeMapRenderPass::RecordCommand(const Scene* scene) {
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

  // recreate uniform buffer
  auto entity = view.front();
  auto& cubeMapComponent = Entity::GetComponent<CubeMapComponent>(scene, entity);

  /**
   * set command
   */

  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->BeginRenderPass(BeginRenderPassInfo{
      .renderPass = m_renderPass,
      .frameBuffer = m_framebuffer,
      .clearColor = {0, 0, 0, 1},
  });
  m_commandBuffer->BindPipeline(m_pipeline);

  const auto& implData = cubeMapComponent.m_implData;
  DLOG_ASSERT(implData->vertexBuffer != nullptr);
  DLOG_ASSERT(implData->indexBuffer != nullptr);

  m_commandBuffer->BindVertexBuffer(implData->vertexBuffer);
  m_commandBuffer->BindIndexBuffer(implData->indexBuffer);
  m_commandBuffer->BindDescriptorSet(BindDescriptorSetInfo{
      .descriptorSet = implData->descriptorSet,
      .layouts = m_descriptorSetLayout,
  });
  m_commandBuffer->DrawIndex(implData->indexBuffer->GetIndexCount(), 0);
  m_commandBuffer->EndRenderPass();
  m_commandBuffer->EndRecordCmd();
}

void
CubeMapRenderPass::CreateBufferForEveryEntity(const entt::entity cubeMap, Scene* scene) {
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
  implData->vertexBuffer = std::move(vertexBuffer);

  auto indexBuffer = m_rhiFactory->CreateIndexBuffer(std::move(indices));
  implData->indexBuffer = std::move(indexBuffer);

  // create descriptor set
  implData->descriptorSet = m_rhiFactory->CreateDescriptorSet(m_descriptorSetLayout);
  implData->descriptorSet->BindBuffer(0, m_cameraUniformBuffer);

  // load material
  if (cubeMapComponent.cubeMapResourceId.has_value()) {
    auto id = cubeMapComponent.cubeMapResourceId.value();
    auto cubeMapResource = m_resourceManager->GetCubeMapResourceContainer()->GetResource(id);
    cubeMapResource->LoadResource(m_rhiFactory, m_resourceManager.get());

    implData->descriptorSet->BindImage(0, cubeMapResource->GetTextureCubeMap());

    implData->textureCubeMapResource = cubeMapResource;
  }
  cubeMapComponent.m_implData = implData;
  m_needToRecordComand = true;
}

void
CubeMapRenderPass::SetUniformBuffer(Scene* scene) {
  // get matrix
  const auto editorCamera = scene->GetEditorCamrea();
  UpdateCameraUniformBuffer(editorCamera.get());
}

void
CubeMapRenderPass::Execute(const Scene* scene, const ResourceManager* resourceMnager) {
  auto view = Entity::GetAllEntity<CubeMapComponent>(scene);
  for (auto&& [entity, cubeMapComponent] : view.each()) {
    CreateBufferForEveryEntity(entity, const_cast<Scene*>(scene));
  }

  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }

  SetUniformBuffer(const_cast<Scene*>(scene));
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
