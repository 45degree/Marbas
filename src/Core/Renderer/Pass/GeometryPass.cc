#include "GeometryPass.hpp"

#include <glog/logging.h>

#include <nameof.hpp>

#include "AssetManager/ModelAsset.hpp"
#include "Core/Common.hpp"
#include "Core/Renderer/GBuffer.hpp"
#include "Core/Scene/Component/Component.hpp"
#include "Core/Scene/GPUDataPipeline/MeshGPUData.hpp"

namespace Marbas {

GeometryPass::GeometryPass(const GeometryPassCreateInfo& createInfo)
    : m_normalMetallicTexture(createInfo.normalMetallicTexture),
      m_positionRoughnessTexture(createInfo.positionRoughnessTexture),
      m_depthTexture(createInfo.depthTexture),
      m_colorTexture(createInfo.colorTexture),
      m_rhiFactory(createInfo.rhiFactory),
      m_width(createInfo.width),
      m_height(createInfo.height) {
  auto pipelineContext = m_rhiFactory->GetPipelineContext();
  auto bufferContext = m_rhiFactory->GetBufferContext();

  SamplerCreateInfo samplerCreateInfo{
      .filter = Marbas::Filter::MIN_MAG_MIP_LINEAR,
      .addressU = Marbas::SamplerAddressMode::WRAP,
      .addressV = Marbas::SamplerAddressMode::WRAP,
      .addressW = Marbas::SamplerAddressMode::WRAP,
      .comparisonOp = Marbas::ComparisonOp::ALWAYS,
      .mipLodBias = 0,
      .minLod = 0,
      .maxLod = 0,
      .borderColor = Marbas::BorderColor::IntOpaqueBlack,
  };
  m_sampler = pipelineContext->CreateSampler(samplerCreateInfo);

  // create camera matrix buffer
  constexpr auto cameraBufferSize = sizeof(CameraMatrix);
  m_cameraBuffer = bufferContext->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_cameraMatrix, cameraBufferSize, false);

  // create empty image
  m_emptyImage = bufferContext->CreateImage(ImageCreateInfo{});
  m_emptyImageView = bufferContext->CreateImageView(ImageViewCreateInfo{
      .image = m_emptyImage,
      .type = ImageViewType::TEXTURE2D,
      .baseLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  });

  // create descirptor
  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
  m_descriptorSet = pipelineContext->CreateDescriptorSet(m_argument);
  pipelineContext->BindBuffer(BindBufferInfo{
      .descriptorSet = m_descriptorSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_cameraBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

void
GeometryPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.WriteTexture(m_colorTexture);
  builder.WriteTexture(m_positionRoughnessTexture);
  builder.WriteTexture(m_normalMetallicTexture);
  builder.WriteTexture(m_depthTexture, TextureAttachmentType::DEPTH);

  builder.BeginPipeline();
  builder.AddShaderArgument(MeshGPUData::GetDescriptorSetArgument());
  builder.AddShaderArgument(m_argument);
  builder.SetPushConstantSize(sizeof(glm::mat4));
  builder.AddShader("Shader/geometry.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/geometry.frag.spv", ShaderType::FRAGMENT_SHADER);
  builder.AddColorTarget({
      .initAction = AttachmentInitAction::CLEAR,  // color
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
      .sampleCount = SampleCount::BIT1,
      .format = GBuffer_Color::format,
  });
  builder.AddColorTarget({
      .initAction = AttachmentInitAction::CLEAR,  // position
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
      .sampleCount = SampleCount::BIT1,
      .format = GBuffer_Position::format,
  });
  builder.AddColorTarget({
      .initAction = AttachmentInitAction::CLEAR,  // normal metaliic roughness
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
      .sampleCount = SampleCount::BIT1,
      .format = GBuffer_Normal_Metallic_Roughness::format,
  });
  builder.SetDepthTarget({
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::DEPTH_STENCIL | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
  });
  builder.SetVertexInputElementDesc(GetMeshVertexInfoLayout());
  builder.SetVertexInputElementView(GetMeshVertexViewInfo());
  builder.AddBlendAttachments({false});
  builder.AddBlendAttachments({false});
  builder.AddBlendAttachments({false});
  builder.SetBlendConstant(0, 0, 0, 0);
  builder.EndPipeline();

  builder.SetFramebufferSize(m_width, m_height, 1);
}

void
GeometryPass::Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandList) {
  auto* scene = registry.GetCurrentActiveScene();

  auto& world = scene->GetWorld();
  auto camera = scene->GetEditorCamera();

  auto* bufferContext = m_rhiFactory->GetBufferContext();
  auto* pipelineContext = m_rhiFactory->GetPipelineContext();

  auto modelManager = AssetManager<ModelAsset>::GetInstance();
  auto meshGPUDataManager = MeshGPUDataManager::GetInstance();

  /**
   * load all model and calculate the sum of mesh
   */

  // update camera buffer
  m_cameraMatrix.up = camera->GetUpVector();
  m_cameraMatrix.pos = camera->GetPosition();
  m_cameraMatrix.right = camera->GetRightVector();
  m_cameraMatrix.projection = camera->GetProjectionMatrix();
  m_cameraMatrix.view = camera->GetViewMatrix();
  bufferContext->UpdateBuffer(m_cameraBuffer, &m_cameraMatrix, sizeof(CameraMatrix), 0);

  /**
   * record command
   */
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();

  std::array<ViewportInfo, 1> viewport;
  viewport[0].x = 0;
  viewport[0].y = 0;
  viewport[0].width = m_width;
  viewport[0].height = m_height;
  viewport[0].minDepth = 0;
  viewport[0].maxDepth = 1;

  std::array<ScissorInfo, 1> scissor;
  scissor[0].x = 0;
  scissor[0].y = 0;
  scissor[0].height = m_height;
  scissor[0].width = m_width;

  commandList.Begin();
  commandList.BeginPipeline(pipeline, framebuffer,
                            {
                                {0, 0, 0, 0},
                                {0, 0, 0, 0},
                                {0, 0, 0, 0},
                                {1, 1},
                            });
  commandList.SetViewports(viewport);
  commandList.SetScissors(scissor);

  auto view = world.view<ModelSceneNode, RenderableTag>();
  for (auto&& [entity, modelSceneNode] : view.each()) {
    glm::mat4 model = glm::mat4(1.0);
    if (world.any_of<TransformComp>(entity)) {
      const auto& transformComp = world.get<TransformComp>(entity);
      model = transformComp.GetGlobalTransform();
    }

    commandList.PushConstant(pipeline, &model, sizeof(glm::mat4), 0);
    for (auto mesh : modelSceneNode.m_meshEntities) {
      if (!world.any_of<RenderableMeshTag>(mesh)) continue;

      auto data = meshGPUDataManager->Get(mesh);
      if (data == nullptr) continue;

      auto& indexCount = data->m_indexCount;

      commandList.BindDescriptorSet(pipeline, {data->m_descriptorSet, m_descriptorSet});
      commandList.BindVertexBuffer(data->m_vertexBuffer);
      commandList.BindIndexBuffer(data->m_indexBuffer);
      commandList.DrawIndexed(indexCount, 1, 0, 0, 0);
    }
  }

  commandList.EndPipeline(pipeline);
  commandList.End();
}

}  // namespace Marbas
