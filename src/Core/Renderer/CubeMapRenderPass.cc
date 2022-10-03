#include "Core/Renderer/CubeMapRenderPass.hpp"

#include <glog/logging.h>

#include <nameof.hpp>

#include "Core/Common.hpp"
#include "Core/Renderer/BlinnPhongRenderPass.hpp"
#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Renderer/HDRImageRenderPass.hpp"
#include "Core/Scene/Component/CubeMapComponent.hpp"

namespace Marbas {

CubeMapRenderPassCreateInfo::CubeMapRenderPassCreateInfo() {
  passName = "CubeMapRenderPass";
  inputPassNode = BlinnPhongRenderPass::renderPassName;
  inputResource = {String(HDRImageRenderPass::targetName)};
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
  DLOG_ASSERT(m_framebuffer != nullptr);
  DLOG_ASSERT(m_renderPass != nullptr);
  DLOG_ASSERT(m_pipeline != nullptr);

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

  m_commandBuffer->BindVertexBuffer(m_vertexBuffer);
  m_commandBuffer->BindIndexBuffer(m_indexBuffer);
  m_commandBuffer->BindDescriptorSet(BindDescriptorSetInfo{
      .descriptorSet = m_descriptorSet,
      .layouts = m_descriptorSetLayout,
  });
  m_commandBuffer->DrawIndex(m_indexBuffer->GetIndexCount(), 0);
  m_commandBuffer->EndRenderPass();
  m_commandBuffer->EndRecordCmd();
}

void
CubeMapRenderPass::BindDescriptorSet(const Scene* scene) {
  const auto& hdrGBuffer = m_inputTarget[String(HDRImageRenderPass::targetName)];
  auto hdrBuffer = hdrGBuffer->GetGBuffer(GBufferTexutreType::HDR_IMAGE);

  m_descriptorSet->BindBuffer(0, m_cameraUniformBuffer);
  m_descriptorSet->BindImage(0, hdrBuffer);
}

void
CubeMapRenderPass::OnInit() {
  const auto& vertices = CubeMapComponent::vertices;
  Vector<uint32_t> indices(CubeMapComponent::indices.begin(), CubeMapComponent::indices.end());

  auto verticesLen = sizeof(Vertex) * vertices.size();
  m_vertexBuffer = m_rhiFactory->CreateVertexBuffer(vertices.data(), verticesLen);
  m_vertexBuffer->SetLayout(GetMeshVertexInfoLayout());
  m_indexBuffer = m_rhiFactory->CreateIndexBuffer(indices);
  m_descriptorSet = m_rhiFactory->CreateDescriptorSet(m_descriptorSetLayout);
}

void
CubeMapRenderPass::SetUniformBuffer(Scene* scene) {
  // get matrix
  const auto editorCamera = scene->GetEditorCamrea();
  UpdateCameraUniformBuffer(editorCamera.get());
}

void
CubeMapRenderPass::Execute(const Scene* scene, const ResourceManager* resourceMnager) {
  BindDescriptorSet(scene);

  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }

  SetUniformBuffer(const_cast<Scene*>(scene));
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
