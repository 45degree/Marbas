#include "SkyImagePass.hpp"

#include <glog/logging.h>

#include <nameof.hpp>

#include "AssetManager/TextureAsset.hpp"
#include "Core/Common.hpp"
#include "Core/Scene/Component/EnvironmentComponent.hpp"
#include "Core/Scene/GPUDataPipeline/TextureGPUData.hpp"

namespace Marbas {

static const std::array<float, 3 * 8> vertices = {
    -1, -1, 1,   //
    1,  -1, 1,   //
    1,  1,  1,   //
    -1, 1,  1,   //
    -1, -1, -1,  //
    1,  -1, -1,  //
    1,  1,  -1,  //
    -1, 1,  -1,  //
};

static const std::array<uint32_t, 36> indices = {
    3, 0, 1,  //
    3, 1, 2,  //
    2, 1, 6,  //
    6, 1, 5,  //
    6, 3, 2,  //
    7, 3, 6,  //
    1, 0, 4,  //
    1, 4, 5,  //
    7, 0, 3,  //
    0, 7, 4,  //
    4, 7, 5,  //
    7, 6, 5,
};

static const std::vector<InputElementDesc> verticesElementDesc{
    {0, ElementType::R32G32B32_SFLOAT, 0, 0, 0},
};

static const std::vector<InputElementView> verticesElementViews = {
    {.binding = 0, .stride = sizeof(float) * 3, .inputClass = VertexInputClass::VERTEX},
};

SkyImagePass::SkyImagePass(const SkyImagePassCreateInfo& createInfo)
    : m_atmosphereTexture(createInfo.atmosphereTexture),
      m_finalColorTexture(createInfo.finalColorTexture),
      m_finalDepthTexture(createInfo.finalDepthTexture),
      m_scene(createInfo.scene),
      m_rhiFactory(createInfo.rhiFactory),
      m_width(createInfo.width),
      m_height(createInfo.height) {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  auto vertexBufSize = vertices.size() * sizeof(float);
  auto indexBufSize = indices.size() * sizeof(uint32_t);
  m_vertexBuffer = bufCtx->CreateBuffer(BufferType::VERTEX_BUFFER, vertices.data(), vertexBufSize, true);
  m_indexBuffer = bufCtx->CreateBuffer(BufferType::INDEX_BUFFER, indices.data(), indexBufSize, true);
  m_cameraInfoUBO = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_cameraInfo, sizeof(CameraInfo), true);
  m_clearUBO = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_clearInfo, sizeof(ClearValueInfo), true);

  SamplerCreateInfo samplerCreateInfo{
      .filter = Marbas::Filter::MIN_MAG_MIP_LINEAR,
      .addressU = Marbas::SamplerAddressMode::CLAMP,
      .addressV = Marbas::SamplerAddressMode::CLAMP,
      .addressW = Marbas::SamplerAddressMode::CLAMP,
      .comparisonOp = Marbas::ComparisonOp::ALWAYS,
      .mipLodBias = 0,
      .minLod = 0,
      .maxLod = 0,
      .borderColor = Marbas::BorderColor::IntOpaqueBlack,
  };
  m_sampler = pipelineCtx->CreateSampler(samplerCreateInfo);

  //
  m_atmosphereArgument.Bind(0, DescriptorType::IMAGE);
  m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
  m_argument.Bind(1, DescriptorType::UNIFORM_BUFFER);

  m_descriptorSet = pipelineCtx->CreateDescriptorSet(m_argument);
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_descriptorSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 0,
      .buffer = m_cameraInfoUBO,
      .offset = 0,
      .arrayElement = 0,
  });
  pipelineCtx->BindBuffer(BindBufferInfo{
      .descriptorSet = m_descriptorSet,
      .descriptorType = DescriptorType::UNIFORM_BUFFER,
      .bindingPoint = 1,
      .buffer = m_clearUBO,
      .offset = 0,
      .arrayElement = 0,
  });
}

SkyImagePass::~SkyImagePass() {
  auto bufCtx = m_rhiFactory->GetBufferContext();
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();

  bufCtx->DestroyBuffer(m_vertexBuffer);
  bufCtx->DestroyBuffer(m_indexBuffer);
  bufCtx->DestroyBuffer(m_cameraInfoUBO);
  pipelineCtx->DestroySampler(m_sampler);
  pipelineCtx->DestroyDescriptorSet(m_descriptorSet);
}

void
SkyImagePass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.ReadTexture(m_atmosphereTexture, m_sampler);
  builder.WriteTexture(m_finalColorTexture);
  builder.WriteTexture(m_finalDepthTexture, TextureAttachmentType::DEPTH);

  builder.BeginPipeline();
  builder.AddShader("Shader/cubeMap.vert.spv", ShaderType::VERTEX_SHADER);
  builder.AddShader("Shader/cubeMap.frag.spv", ShaderType::FRAGMENT_SHADER);
  builder.AddColorTarget({
      .initAction = AttachmentInitAction::KEEP,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::COLOR_RENDER_TARGET | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
      .format = ImageFormat::RGBA,
  });
  builder.SetDepthTarget({
      .initAction = AttachmentInitAction::KEEP,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::DEPTH_STENCIL | ImageUsageFlags::SHADER_READ,
      .sampleCount = SampleCount::BIT1,
  });
  builder.SetBlendConstant(0, 0, 0, 1);
  builder.AddBlendAttachments({.blendEnable = false});
  builder.EnableDepthTest(true);
  builder.SetDepthCampareOp(DepthCompareOp::LEQUAL);
  builder.SetVertexInputElementDesc(verticesElementDesc);
  builder.SetVertexInputElementView(verticesElementViews);
  builder.AddShaderArgument(m_argument);
  builder.AddShaderArgument(m_atmosphereArgument);
  builder.EndPipeline();

  builder.SetFramebufferSize(m_width, m_height, 1);
}

void
SkyImagePass::Execute(RenderGraphRegistry& registry, GraphicsCommandBuffer& commandList) {
  auto& world = m_scene->GetWorld();
  auto view = world.view<EnvironmentComponent>();
  if (view.size() == 0) return;

  auto entity = view[0];
  auto& component = world.get<EnvironmentComponent>(entity);

  // check framebuffer and renderpass
  auto pipeline = registry.GetPipeline(0);
  auto framebuffer = registry.GetFrameBuffer();
  auto atmosphereSet = registry.GetInputDescriptorSet();

  // set camera info
  auto camera = m_scene->GetEditorCamera();
  auto* bufCtx = m_rhiFactory->GetBufferContext();
  m_cameraInfo.view = camera->GetViewMatrix();
  m_cameraInfo.projection = camera->GetProjectionMatrix();
  bufCtx->UpdateBuffer(m_cameraInfoUBO, &m_cameraInfo, sizeof(CameraInfo), 0);

  // set clear value
  m_clearInfo.isClear = component.currentItem == EnvironmentComponent::clearValueItem;
  m_clearInfo.clearValue[0] = component.clearValueSky.clearValue[0];
  m_clearInfo.clearValue[1] = component.clearValueSky.clearValue[1];
  m_clearInfo.clearValue[2] = component.clearValueSky.clearValue[2];
  m_clearInfo.clearValue[3] = component.clearValueSky.clearValue[3];
  bufCtx->UpdateBuffer(m_clearUBO, &m_clearInfo, sizeof(ClearValueInfo), 0);

  // bind hdr image
  // FIX: 绑定了image后无法还原
  if (component.currentItem == EnvironmentComponent::imageSkyItem) {
    const auto& assetPath = component.imageSky.hdrImagePath;
    auto textureAssetMgr = AssetManager<TextureAsset>::GetInstance();

    if (!textureAssetMgr->Existed(assetPath)) {
      textureAssetMgr->Create(assetPath, true);
    }
    auto textureAsset = textureAssetMgr->Get(assetPath);
    auto gpuTextureAssetMgr = TextureGPUDataManager::GetInstance();
    if (!gpuTextureAssetMgr->Existed(*textureAsset)) {
      gpuTextureAssetMgr->Create(*textureAsset);
    }
    auto gpuAsset = gpuTextureAssetMgr->TryGet(*textureAsset);
    auto imageView = gpuAsset->GetImageView(0, 1, 0, 1);

    auto pipelineCtx = m_rhiFactory->GetPipelineContext();
    pipelineCtx->BindImage(BindImageInfo{
        .descriptorSet = atmosphereSet,
        .bindingPoint = 0,
        .imageView = imageView,
        .sampler = m_sampler,
    });
  }

  /**
   * record command
   */
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
  commandList.BeginPipeline(pipeline, framebuffer, {{0, 0, 0, 0}, {1, 1}});
  commandList.SetViewports(viewport);
  commandList.SetScissors(scissor);
  commandList.BindVertexBuffer(m_vertexBuffer);
  commandList.BindIndexBuffer(m_indexBuffer);
  commandList.BindDescriptorSet(pipeline, {m_descriptorSet, atmosphereSet});
  commandList.DrawIndexed(indices.size(), 1, 0, 0, 0);
  commandList.EndPipeline(pipeline);
  commandList.End();
}

bool
SkyImagePass::IsEnable() {
  auto& world = m_scene->GetWorld();
  auto view = world.view<EnvironmentComponent>();
  if (view.size() == 0) return false;

  auto entity = view[0];

  auto& component = world.get<EnvironmentComponent>(entity);
  if (component.currentItem == EnvironmentComponent::imageSkyItem) {
    if (component.imageSky.hdrImagePath == "res://") return false;
  }

  return true;
}

}  // namespace Marbas
