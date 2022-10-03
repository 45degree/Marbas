#include "Core/Renderer/PrefilterRenderPass.hpp"

#include "Core/Common.hpp"
#include "Core/Renderer/HDRImageRenderPass.hpp"
#include "Core/Scene/Component/CubeMapComponent.hpp"

namespace Marbas {

PrefilterRenderPassCreateInfo::PrefilterRenderPassCreateInfo() : DeferredRenderPassCreateInfo() {
  passName = "prefilter render pass";
  inputResource = {String(HDRImageRenderPass::targetName)};
  outputResource = {String(PrefilterRenderPass::targetName)};
}

PrefilterRenderPass::PrefilterRenderPass(const PrefilterRenderPassCreateInfo &createInfo)
    : DeferredRenderPass(createInfo) {
  for (int i = 0; i < maxMipmapLevel; i++) {
    m_commandBuffers[i] = m_rhiFactory->CreateCommandBuffer();
  }
}

void
PrefilterRenderPass::CreateFrameBuffer() {
  auto targetGBuffer = m_outputTarget[String(targetName)];
  auto targetBuffer = targetGBuffer->GetGBuffer(GBufferTexutreType::PRE_FILTER_CUBEMAP);
  for (uint32_t i = 0; i < maxMipmapLevel; i++) {
    m_frameBufferImageViews[i] = m_rhiFactory->CreateImageView(ImageViewDesc{
        .m_texture = targetBuffer,
        .m_format = targetBuffer->GetFormat(),
        .m_type = targetBuffer->GetTextureType(),
        .m_layerBase = 0,
        .m_layerCount = 6,
        .m_levelBase = i,
        .m_levelCount = 1,
    });
    m_frameBuffers[i] = m_rhiFactory->CreateFrameBuffer(FrameBufferInfo{
        .width = static_cast<uint32_t>(128 * std::pow(0.5, i)),
        .height = static_cast<uint32_t>(128 * std::pow(0.5, i)),
        .renderPass = m_renderPass.get(),
        .attachments = {m_frameBufferImageViews[i]},
    });
  }
}

void
PrefilterRenderPass::OnInit() {
  auto vertices = CubeMapComponent::vertices;
  constexpr auto verticesSize = sizeof(vertices[0]) * vertices.size();
  Vector<uint32_t> indices(CubeMapComponent::indices.begin(), CubeMapComponent::indices.end());

  m_viewMatrix.projectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  m_viewMatrix.viewMatrix[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                                           glm::vec3(0.0f, -1.0f, 0.0f));
  m_viewMatrix.viewMatrix[1] = glm::lookAt(
      glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
  m_viewMatrix.viewMatrix[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                                           glm::vec3(0.0f, 0.0f, 1.0f));
  m_viewMatrix.viewMatrix[3] = glm::lookAt(
      glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
  m_viewMatrix.viewMatrix[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                                           glm::vec3(0.0f, -1.0f, 0.0f));
  m_viewMatrix.viewMatrix[5] = glm::lookAt(
      glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

  m_vertexBuffer = m_rhiFactory->CreateVertexBuffer(vertices.data(), verticesSize);
  m_vertexBuffer->SetLayout(GetMeshVertexInfoLayout());
  m_indexBuffer = m_rhiFactory->CreateIndexBuffer(indices);
  m_descriptorSet = m_rhiFactory->CreateDescriptorSet(m_descriptorSetLayout);
  m_ViewMatrixUBO = m_rhiFactory->CreateUniformBuffer(sizeof(ViewMatrix));
  m_ViewMatrixUBO->SetData(&m_viewMatrix, sizeof(ViewMatrix), 0);

  m_descriptorSet->BindBuffer(1, m_ViewMatrixUBO);
}

void
PrefilterRenderPass::CreateRenderPass() {
  m_renderPass = m_rhiFactory->CreateRenderPass(RenderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
                  .format = TextureFormat::RGB16F,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
          },
  });
}

void
PrefilterRenderPass::CreateShader() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/prefilter.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::GEOMETRY_SHADER, "Shader/prefilter.geom.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/prefilter.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);
}

void
PrefilterRenderPass::CreateDescriptorSetLayout() {
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = true,
      .type = BufferDescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 1,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = false,
      .bindingPoint = 0,
  });
}

void
PrefilterRenderPass::RecordCommand(const Scene *scene) {
  DLOG_ASSERT(m_renderPass != nullptr);
  DLOG_ASSERT(m_pipeline != nullptr);

  for (int i = 0; i < maxMipmapLevel; i++) {
    m_commandBuffers[i]->BeginRecordCmd();
    m_commandBuffers[i]->BeginRenderPass(BeginRenderPassInfo{
        .renderPass = m_renderPass,
        .frameBuffer = m_frameBuffers[i],
        .clearColor = {0, 0, 0, 1},
    });
    m_commandBuffers[i]->BindPipeline(m_pipeline);
    m_commandBuffers[i]->BindVertexBuffer(m_vertexBuffer);
    m_commandBuffers[i]->BindIndexBuffer(m_indexBuffer);
    m_commandBuffers[i]->BindDescriptorSet(BindDescriptorSetInfo{
        .descriptorSet = m_descriptorSet,
        .layouts = m_descriptorSetLayout,
    });

    auto width = static_cast<uint32_t>(128 * std::pow(0.5, i));
    auto height = static_cast<uint32_t>(128 * std::pow(0.5, i));
    m_commandBuffers[i]->SetViewports({ViewportInfo{
        .x = 0,
        .y = 0,
        .width = width,
        .height = height,
    }});
    m_commandBuffers[i]->DrawIndex(CubeMapComponent::indices.size(), 0);
    m_commandBuffers[i]->EndRenderPass();
    m_commandBuffers[i]->EndRecordCmd();
  }
}

void
PrefilterRenderPass::CreatePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto resource = shaderContainer->GetResource(m_shaderId);
  if (!resource->IsLoad()) {
    resource->LoadResource(m_rhiFactory, m_resourceManager.get());
  }

  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetShader(resource->GetShader());
  m_pipeline->SetVertexBufferLayout(GetMeshVertexInfoLayout(), VertexInputRate::VERTEX);
  m_pipeline->SetDepthStencilInfo(DepthStencilInfo{
      .depthTestEnable = false,
  });
  m_pipeline->Create();
}

void
PrefilterRenderPass::SetUniformBuffer(Scene *scene) {
  // get matrix
  const auto editorCamera = scene->GetEditorCamrea();
  UpdateCameraUniformBuffer(editorCamera.get());
}

void
PrefilterRenderPass::Execute(const Scene *scene, const ResourceManager *resourceManager) {
  const auto &targetGBuffer = m_inputTarget[String(HDRImageRenderPass::targetName)];
  auto targetBuffer = targetGBuffer->GetGBuffer(GBufferTexutreType::HDR_IMAGE);

  m_descriptorSet->BindImage(0, targetBuffer);
  m_descriptorSet->BindBuffer(0, m_cameraUniformBuffer);
  m_descriptorSet->BindBuffer(1, m_ViewMatrixUBO);

  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }

  for (int i = 0; i < maxMipmapLevel; i++) {
    SetUniformBuffer(const_cast<Scene *>(scene));
    m_viewMatrix.roughness = (float)i / (float)(maxMipmapLevel - 1);
    m_ViewMatrixUBO->SetData(&m_viewMatrix, sizeof(ViewMatrix), 0);
    m_commandBuffers[i]->SubmitCommand();
  }
}

}  // namespace Marbas
