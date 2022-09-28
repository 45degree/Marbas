#include "Core/Renderer/IrradianceRenderPass.hpp"

#include "Core/Common.hpp"
#include "Core/Renderer/BeginningRenderPass.hpp"
#include "Core/Renderer/HDRImageRenderPass.hpp"
#include "Core/Scene/Component/CubeMapComponent.hpp"
#include "Core/Scene/Component/EnvironmentComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"
#include "Core/Scene/System/LoadImpl.hpp"

namespace Marbas {

IrradianceRenderPassCreateInfo::IrradianceRenderPassCreateInfo() : DeferredRenderPassCreateInfo() {
  passName = String(IrradianceRenderPass::renderPassName);
  inputResource = {String(HDRImageRenderPass::targetName)};
  outputResource = {String(IrradianceRenderPass::targetName)};
}

IrradianceRenderPass::IrradianceRenderPass(const IrradianceRenderPassCreateInfo& createInfo)
    : DeferredRenderPass(createInfo) {}

void
IrradianceRenderPass::CreateRenderPass() {
  RenderPassCreateInfo createInfo{
      .attachments =
          {
              AttachmentDescription{
                  .format = TextureFormat::RGB16F,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Clear,
              },
          },
  };

  m_renderPass = m_rhiFactory->CreateRenderPass(createInfo);
}

void
IrradianceRenderPass::CreateDescriptorSetLayout() {
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = false,
      .bindingPoint = 0,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      .isBuffer = true,
      .type = BufferDescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 1,
  });
}

void
IrradianceRenderPass::CreateFrameBuffer() {
  const auto& targetGBuffer = m_outputTarget[String(targetName)]->GetGBuffer();
  auto targetBuffer = targetGBuffer->GetTexture(GBufferTexutreType::HDR_IMAGE);

  std::shared_ptr hdrImageView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = targetBuffer,
      .m_format = targetBuffer->GetFormat(),
      .m_type = targetBuffer->GetTextureType(),
      .m_layerBase = 0,
      .m_layerCount = 6,
  });

  FrameBufferInfo createInfo{
      .width = 32,
      .height = 32,
      .renderPass = m_renderPass.get(),
      .attachments = {hdrImageView},
  };
  m_framebuffer = m_rhiFactory->CreateFrameBuffer(createInfo);
}

void
IrradianceRenderPass::CreatePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto resource = shaderContainer->GetResource(m_shaderId);
  if (!resource->IsLoad()) {
    resource->LoadResource(m_rhiFactory, m_resourceManager.get());
  }

  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = 32, .height = 32});
  m_pipeline->SetShader(resource->GetShader());
  m_pipeline->SetVertexBufferLayout(GetMeshVertexInfoLayout(), VertexInputRate::VERTEX);
  m_pipeline->SetDepthStencilInfo(DepthStencilInfo{
      .depthTestEnable = false,
  });
  m_pipeline->Create();
}

void
IrradianceRenderPass::CreateShader() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/irradiance.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::GEOMETRY_SHADER, "Shader/irradiance.geom.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/irradiance.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);
}

void
IrradianceRenderPass::OnInit() {
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
IrradianceRenderPass::RecordCommand(const Scene* scene) {
  auto view = Entity::GetAllEntity<EnvironmentComponent>(scene);
  DLOG_ASSERT(view.size() == 1);
  DLOG_ASSERT(m_framebuffer != nullptr);
  DLOG_ASSERT(m_renderPass != nullptr);
  DLOG_ASSERT(m_pipeline != nullptr);

  auto& environmentComponent = Entity::GetComponent<EnvironmentComponent>(scene, view[0]);
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
  m_commandBuffer->DrawIndex(CubeMapComponent::indices.size(), 0);
  m_commandBuffer->EndRenderPass();
  m_commandBuffer->EndRecordCmd();
}

void
IrradianceRenderPass::SetUniformBuffer(Scene* scene) {
  // get matrix
  const auto editorCamera = scene->GetEditorCamrea();
  UpdateCameraUniformBuffer(editorCamera.get());
}

void
IrradianceRenderPass::Execute(const Scene* scene, const ResourceManager* resourceManager) {
  // bind image

  const auto& targetGBuffer = m_inputTarget[String(HDRImageRenderPass::targetName)]->GetGBuffer();
  auto targetBuffer = targetGBuffer->GetTexture(GBufferTexutreType::HDR_IMAGE);

  m_descriptorSet->BindImage(0, targetBuffer);
  m_descriptorSet->BindBuffer(0, m_cameraUniformBuffer);

  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }

  SetUniformBuffer(const_cast<Scene*>(scene));
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
