#include "RenderSceneFromProbe.hpp"

#include "Core/Common.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/RenderMeshComponent.hpp"
#include "Core/Scene/Scene.hpp"

namespace Marbas {

RenderSceneFromProbe::DescriptorComponent::DescriptorComponent(RHIFactory* rhiFactory,
                                                               DescriptorSetLayout* descriptorSetLayout)
    : rhiFactory(rhiFactory) {
  auto pipelineCtx = rhiFactory->GetPipelineContext();

  std::array poolSize = {
      DescriptorPoolSize{DescriptorType::UNIFORM_BUFFER, 2},
  };
  pool = pipelineCtx->CreateDescriptorPool(poolSize, 200);
  set = pipelineCtx->CreateDescriptorSet(pool, descriptorSetLayout);
}

RenderSceneFromProbe::DescriptorComponent::~DescriptorComponent() {
  auto pipelineCtx = rhiFactory->GetPipelineContext();
  pipelineCtx->DestroyDescriptorSet(pool, set);
  pipelineCtx->DestroyDescriptorPool(pool);
}

RenderSceneFromProbe::RenderSceneFromProbe(RHIFactory* rhiFactory) : m_rhiFactory(rhiFactory) {
  auto bufCtx = m_rhiFactory->GetBufferContext();

  m_commandPool = bufCtx->CreateCommandPool(CommandBufferUsage::GRAPHICS);
  m_commandBuffer = bufCtx->CreateCommandBuffer(m_commandPool);

  m_descriptorSetLayout = m_rhiFactory->GetPipelineContext()->CreateDescriptorSetLayout({
      {.bindingPoint = 0, .descriptorType = DescriptorType::UNIFORM_BUFFER},
      {.bindingPoint = 1, .descriptorType = DescriptorType::UNIFORM_BUFFER},
  });

  m_cameraBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_cameraInfo, sizeof(CameraInfo), false);

  m_fence = m_rhiFactory->CreateFence();

  CreateShader();
  CreatePipeline();
}

void
RenderSceneFromProbe::Render(const RenderInfo& renderInfo) {
  m_cameraInfo.project = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
  m_cameraInfo.view[0] = glm::lookAt(m_probePos, {1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f});
  m_cameraInfo.view[1] = glm::lookAt(m_probePos, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f});
  m_cameraInfo.view[2] = glm::lookAt(m_probePos, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
  m_cameraInfo.view[3] = glm::lookAt(m_probePos, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f});
  m_cameraInfo.view[4] = glm::lookAt(m_probePos, {0.0f, 0.0f, 1.0f}, {0.0f, -1.0f, 0.0f});
  m_cameraInfo.view[5] = glm::lookAt(m_probePos, {0.0f, 0.0f, -1.0f}, {0.0f, -1.0f, 0.0f});
  m_rhiFactory->GetBufferContext()->UpdateBuffer(m_cameraBuffer, &m_cameraInfo, sizeof(CameraInfo), 0);

  //  create frame buffer
  FrameBufferCreateInfo createInfo;
  createInfo.width = renderInfo.width;
  createInfo.height = renderInfo.height;
  createInfo.layer = 6;
  createInfo.pieline = m_pipeline;
  createInfo.attachments.colorAttachments = {renderInfo.posCubemap, renderInfo.normalCubemap};
  createInfo.attachments.depthAttachment = renderInfo.depthCubemap;

  m_rhiFactory->ResetFence(m_fence);

  auto* frameBuffer = m_rhiFactory->GetPipelineContext()->CreateFrameBuffer(createInfo);
  RecordCommand(renderInfo.scene, frameBuffer);
  m_commandBuffer->Submit(std::span<Semaphore*>(), std::span<Semaphore*>(), m_fence);

  m_rhiFactory->WaitForFence(m_fence);
}

void
RenderSceneFromProbe::CreateShader() {
  // read shader
  // auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  // auto shaderResource = shaderContainer->CreateResource(HashMap<ShaderType, Path>{
  //     {ShaderType::VERTEX_SHADER, "Shader/RenderSceneFromProbe.vert.glsl.spv"},
  //     {ShaderType::GEOMETRY_SHADER, "Shader/RenderSceneFromProbe.geom.glsl.spv"},
  //     {ShaderType::FRAGMENT_SHADER, "Shader/RenderSceneFromProbe.frag.glsl.spv"},
  // });
  // shaderResource->LoadResource(m_resourceManager);
  // m_shaderId = shaderContainer->AddResource(shaderResource);
}

void
RenderSceneFromProbe::LoadRenderData(entt::registry& world, entt::entity entity) {
  // auto* pipelineContext = m_rhiFactory->GetPipelineContext();
  // auto* bufferContext = m_rhiFactory->GetBufferContext();
  //
  // if (world.any_of<RenderMeshComponent>(entity)) {
  //   auto& renderComp = world.get<RenderMeshComponent>(entity);
  //
  //   if (!world.any_of<DescriptorComponent>(entity)) {
  //     auto& component = world.emplace<DescriptorComponent>(entity, m_rhiFactory, m_descriptorSetLayout);
  //
  //     /**
  //      * bind descriptor set
  //      */
  //     pipelineContext->BindBuffer(BindBufferInfo{
  //         .descriptorSet = component.set,
  //         .descriptorType = DescriptorType::UNIFORM_BUFFER,
  //         .bindingPoint = 1,
  //         .buffer = m_cameraBuffer,
  //         .offset = 0,
  //         .arrayElement = 0,
  //     });
  //
  //     pipelineContext->BindBuffer(BindBufferInfo{
  //         .descriptorSet = component.set,
  //         .descriptorType = DescriptorType::UNIFORM_BUFFER,
  //         .bindingPoint = 0,
  //         .buffer = renderComp.infoBuffer,
  //         .offset = 0,
  //         .arrayElement = 0,
  //     });
  //   }
  // }
}

void
RenderSceneFromProbe::CreatePipeline() {
  // auto shaderResourceContainer = m_resourceManager->GetShaderResourceContainer();
  // auto shaderResource = shaderResourceContainer->GetResource(m_shaderId);
  // if (!shaderResource->IsLoad()) {
  //   shaderResource->LoadResource(m_resourceManager);
  // }
  // const auto& shaderModules = shaderResource->GetShaderCodes();
  // Vector<ShaderStageCreateInfo> shaderStageCreateInfos;
  // for (const auto& [shaderType, shaderModule] : shaderModules) {
  //   shaderStageCreateInfos.push_back(ShaderStageCreateInfo{
  //       .stage = shaderType,
  //       .code = shaderModule,
  //       .interName = "main",
  //   });
  // }

  /**
   * create graphics pipeline
   */
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  GraphicsPipeLineCreateInfo createInfo;
  // createInfo.layout = m_descriptorSetLayout;
  // createInfo.shaderStageCreateInfo = shaderStageCreateInfos;
  // createInfo.blendInfo.constances = {0, 0, 0, 0};
  // createInfo.blendInfo.attachments = {
  //     BlendAttachment{.blendEnable = false},
  //     BlendAttachment{.blendEnable = false},
  // };
  // createInfo.depthStencilInfo.depthTestEnable = true;
  // createInfo.vertexInputLayout.viewDesc = GetMeshVertexViewInfo();
  // createInfo.vertexInputLayout.elementDesc = GetMeshVertexInfoLayout();
  // createInfo.outputRenderTarget.colorAttachments = {
  //     ColorTargetDesc{
  //         .initAction = AttachmentInitAction::CLEAR,
  //         .finalAction = AttachmentFinalAction::READ,
  //         .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
  //         .sampleCount = SampleCount::BIT1,
  //         .format = ImageFormat::RGBA32F,
  //     },
  //     ColorTargetDesc{
  //         .initAction = AttachmentInitAction::CLEAR,
  //         .finalAction = AttachmentFinalAction::READ,
  //         .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
  //         .sampleCount = SampleCount::BIT1,
  //         .format = ImageFormat::RGBA32F,
  //     },
  // };
  // createInfo.outputRenderTarget.depthAttachments = DepthTargetDesc{
  //     .initAction = AttachmentInitAction::CLEAR,
  //     .finalAction = AttachmentFinalAction::DISCARD,
  //     .usage = ImageUsageFlags::DEPTH_STENCIL,
  //     .sampleCount = SampleCount::BIT1,
  // };

  m_pipeline = pipelineCtx->CreatePipeline(createInfo);
}

void
RenderSceneFromProbe::RecordCommand(Scene* scene, FrameBuffer* frameBuffer) {
  // std::array<ViewportInfo, 1> viewPortInfo = {
  //     ViewportInfo{0, 0, static_cast<float>(frameBuffer->width), static_cast<float>(frameBuffer->height), 0, 1},
  // };
  // std::array<ScissorInfo, 1> scissorInfo = {
  //     ScissorInfo{0, 0, frameBuffer->width, frameBuffer->height},
  // };
  // m_commandBuffer->Begin();
  // m_commandBuffer->BeginPipeline(m_pipeline, frameBuffer, {{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 0}});
  // m_commandBuffer->SetViewports(viewPortInfo);
  // m_commandBuffer->SetScissors(scissorInfo);
  //
  // auto& registry = scene->GetWorld();
  // auto camera = scene->GetEditorCamera();
  // auto view = registry.view<RenderMeshComponent>();
  //
  // for (auto&& [entity, renderMeshComponent] : view.each()) {
  //   auto& indexCount = renderMeshComponent.indexCount;
  //   LoadRenderData(scene->GetWorld(), entity);
  //   auto& descriptorComponent = scene->GetWorld().get<DescriptorComponent>(entity);
  //
  //   m_commandBuffer->BindVertexBuffer(renderMeshComponent.vertexBuffer);
  //   m_commandBuffer->BindIndexBuffer(renderMeshComponent.indexBuffer);
  //   m_commandBuffer->BindDescriptorSet(m_pipeline, descriptorComponent.set);
  //   m_commandBuffer->DrawIndexed(indexCount, 1, 0, 0, 0);
  // }
  // m_commandBuffer->EndPipeline(m_pipeline);
  // m_commandBuffer->End();
}

}  // namespace Marbas
