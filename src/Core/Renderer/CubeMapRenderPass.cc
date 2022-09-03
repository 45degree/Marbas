#include "Core/Renderer/CubeMapRenderPass.hpp"

#include <glog/logging.h>

#include <nameof.hpp>

#include "Core/Renderer/BlinnPhongRenderPass.hpp"
#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Renderer/PointLightShadowMappingRenderPass.hpp"
#include "Core/Renderer/ShadowMappingRenderPass.hpp"
#include "Core/Scene/Component/CubeMapComponent.hpp"

namespace Marbas {

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

CubeMapRenderPassCreateInfo::CubeMapRenderPassCreateInfo() {
  passName = "CubeMapRenderPass";
  inputPassNode = PointLightShadowMappingRenderPass::renderPassName;
}

CubeMapRenderPass::CubeMapRenderPass(const CubeMapRenderPassCreateInfo& createInfo)
    : ForwardRenderPass(createInfo) {
  DLOG_ASSERT(m_rhiFactory != nullptr)
      << FORMAT("can't Initialize the {}, because the rhiFactory isn't been set",
                NAMEOF_TYPE(CubeMapRenderPass));
  DLOG_ASSERT(m_resourceManager != nullptr)
      << FORMAT("can't Initialize the {}, because the resource manager isn't been set",
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

  // read shader
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/cubeMap.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/cubeMap.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);

  // set descriptor set layout and create pipeline
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = false,
      .bindingPoint = 0,
  });
  GeneratePipeline();
}

void
CubeMapRenderPass::GeneratePipeline() {
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

  /**
   * begin to record command
   */

  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->AddCommand(std::move(beginRenderPass));
  m_commandBuffer->AddCommand(std::move(bindPipeline));

  const auto& implData = cubeMapComponent.m_implData;
  DLOG_ASSERT(implData->vertexBuffer != nullptr);
  DLOG_ASSERT(implData->indexBuffer != nullptr);

  auto bindVertexBuffer = m_commandFactory->CreateBindVertexBufferCMD();
  auto bindIndexBuffer = m_commandFactory->CreateBindIndexBufferCMD();
  auto bindDescriptorSet = m_commandFactory->CreateBindDescriptorSetCMD();
  auto drawIndex = m_commandFactory->CreateDrawIndexCMD(m_pipeline);

  bindVertexBuffer->SetVertexBuffer(implData->vertexBuffer);
  bindIndexBuffer->SetIndexBuffer(implData->indexBuffer);
  bindDescriptorSet->SetDescriptor(implData->descriptorSet);

  drawIndex->SetIndexCount(implData->indexBuffer->GetIndexCount());

  m_commandBuffer->AddCommand(std::move(bindVertexBuffer));
  m_commandBuffer->AddCommand(std::move(bindIndexBuffer));
  m_commandBuffer->AddCommand(std::move(bindDescriptorSet));
  m_commandBuffer->AddCommand(std::move(drawIndex));

  m_commandBuffer->AddCommand(std::move(endRenderPass));
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
  implData->descriptorSet = GenerateDescriptorSet();
  BindCameraUniformBuffer(implData->descriptorSet.get());

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