#include "VoxelizationPass.hpp"

#include "Core/Common.hpp"
#include "Core/Scene/GPUDataPipeline/LightGPUData.hpp"
#include "Core/Scene/GPUDataPipeline/MeshGPUData.hpp"

namespace Marbas::GI {

VoxelizationPass::VoxelizationPass(const VoxelizationCreateInfo& createInfo)
    : m_shadowMap(createInfo.shadowMap), m_voxelScene(createInfo.voxelScene), m_rhiFactory(createInfo.rhiFactory) {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto bufCtx = m_rhiFactory->GetBufferContext();

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
  m_sampler = pipelineCtx->CreateSampler(samplerCreateInfo);

  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
  m_set = pipelineCtx->CreateDescriptorSet(m_argument);

  m_voxelInfoBuffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_voxelInfo, sizeof(VoxelInfo), false);
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_set,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_voxelInfoBuffer,
      .offset = 0,
      .arrayElement = 0,
  });
}

VoxelizationPass::~VoxelizationPass() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  bufCtx->DestroyBuffer(m_voxelInfoBuffer);
  pipelineCtx->DestroyDescriptorSet(m_set);
  pipelineCtx->DestroySampler(m_sampler);
}

void
VoxelizationPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.ReadTexture(m_shadowMap, m_sampler);
  builder.ReadStorageImage(m_voxelScene);

  DescriptorSetArgument m_inputGBufferArgument;
  m_inputGBufferArgument.Bind(0, DescriptorType::IMAGE);
  m_inputGBufferArgument.Bind(1, DescriptorType::STORAGE_IMAGE);

  builder.BeginPipeline();
  builder.AddShaderArgument(m_inputGBufferArgument);
  builder.AddShaderArgument(m_argument);
  builder.AddShaderArgument(MeshGPUData::GetDescriptorSetArgument());
  builder.AddShaderArgument(LightGPUData::GetDescriptorSetArgument());
  builder.SetPushConstantSize(sizeof(glm::mat4));
  builder.EnableDepthTest(false);
  builder.SetCullMode(CullMode::NONE);
  builder.AddShader("Shader/voxelization.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/voxelization.geom.spv", ShaderType::GEOMETRY_SHADER);
  builder.AddShader("Shader/voxelization.frag.spv", ShaderType::FRAGMENT_SHADER);
  builder.SetVertexInputElementDesc(GetMeshVertexInfoLayout());
  builder.SetVertexInputElementView(GetMeshVertexViewInfo());
  builder.EndPipeline();

  builder.SetFramebufferSize(256, 256, 1);
}

void
VoxelizationPass::Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandBuffer) {
  auto set = registry.GetInputDescriptorSet();
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();

  auto* voxelSceneImage = registry.GetImage(m_voxelScene);

  auto* scene = registry.GetCurrentActiveScene();
  auto& world = scene->GetWorld();

  auto* meshGPUDataManager = MeshGPUDataManager::GetInstance();

  /**
   * update uniform buffer
   */
  float halfSize = 1500;
  auto projectMatrix = glm::ortho(-halfSize, halfSize, -halfSize, halfSize, 0.f, 2 * halfSize);
  projectMatrix[1][1] *= -1;
  auto viewX = glm::lookAt(glm::vec3(halfSize, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  auto viewY = glm::lookAt(glm::vec3(0, halfSize, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
  auto viewZ = glm::lookAt(glm::vec3(0, 0, halfSize), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
  m_voxelInfo.projX = projectMatrix * viewX;
  m_voxelInfo.projY = projectMatrix * viewY;
  m_voxelInfo.projZ = projectMatrix * viewZ;
  m_voxelInfo.viewMatrix = scene->GetEditorCamera()->GetViewMatrix();

  auto bufCtx = m_rhiFactory->GetBufferContext();
  bufCtx->UpdateBuffer(m_voxelInfoBuffer, &m_voxelInfo, sizeof(VoxelInfo), 0);

  /**
   * record command
   */

  commandBuffer.Begin();
  commandBuffer.ClearColorImage(voxelSceneImage, {0, 0, 0, 0}, 0, 1, 0, 1);
  commandBuffer.BeginPipeline(pipeline, framebuffer, {});

  std::array<ViewportInfo, 1> viewportInfo;
  viewportInfo[0].x = 0;
  viewportInfo[0].y = 0;
  viewportInfo[0].width = m_voxelInfo.voxelResolution;
  viewportInfo[0].height = m_voxelInfo.voxelResolution;

  std::array<ScissorInfo, 1> scissorInfo;
  scissorInfo[0].x = 0;
  scissorInfo[0].y = 0;
  scissorInfo[0].width = m_voxelInfo.voxelResolution;
  scissorInfo[0].height = m_voxelInfo.voxelResolution;

  commandBuffer.SetViewports(viewportInfo);
  commandBuffer.SetScissors(scissorInfo);

  auto view = world.view<ModelSceneNode, RenderableTag>();
  for (auto&& [modelNode, modelNodeComp] : view.each()) {
    glm::mat4 modelTransform(1.0);
    if (world.any_of<TransformComp>(modelNode)) {
      modelTransform = world.get<TransformComp>(modelNode).GetGlobalTransform();
    }

    commandBuffer.PushConstant(pipeline, &modelTransform, sizeof(glm::mat4), 0);
    for (auto meshEntity : modelNodeComp.m_meshEntities) {
      if (!world.any_of<RenderableMeshTag>(meshEntity)) continue;

      auto data = meshGPUDataManager->TryGet(meshEntity);
      if (data == nullptr) continue;

      commandBuffer.BindDescriptorSet(pipeline, {set, m_set, data->m_descriptorSet, LightGPUData::GetLightSet()});
      commandBuffer.BindVertexBuffer(data->m_vertexBuffer);
      commandBuffer.BindIndexBuffer(data->m_indexBuffer);
      commandBuffer.DrawIndexed(data->m_indexCount, 1, 0, 0, 0);
    }
  }

  // TODO:
  commandBuffer.EndPipeline(pipeline);
  commandBuffer.End();
}

};  // namespace Marbas::GI
