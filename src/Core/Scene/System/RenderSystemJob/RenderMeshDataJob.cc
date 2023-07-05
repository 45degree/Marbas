#include "RenderMeshDataJob.hpp"

#include "RenderSystem.hpp"

namespace Marbas::Job {

RenderMeshDataJob::RenderMeshDataJob(RHIFactory* rhiFactory) : m_rhiFactory(rhiFactory) {}

RenderMeshDataJob::~RenderMeshDataJob() {
  auto pipelineCtx = m_rhiFactory->GetPipelineContext();
  auto bufCtx = m_rhiFactory->GetBufferContext();

  pipelineCtx->DestroySampler(m_sampler);
  bufCtx->DestroyImageView(m_emptyImageView);
  bufCtx->DestroyImage(m_emptyImage);
}

void
RenderMeshDataJob::init() {
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
  m_emptyImage = bufCtx->CreateImage(ImageCreateInfo{
      .width = 16,
      .height = 16,
      .format = ImageFormat::RGBA,
      .sampleCount = SampleCount::BIT1,
      .mipMapLevel = 1,
      .usage = ImageUsageFlags::SHADER_READ,
      .imageDesc = Image2DDesc(),
  });
  m_emptyImageView = bufCtx->CreateImageView(ImageViewCreateInfo{
      .image = m_emptyImage,
      .type = ImageViewType::TEXTURE2D,
      .baseLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  });
}

void
RenderMeshDataJob::update(DeltaTime deltaTime, void* data) {
  auto* renderInfo = reinterpret_cast<RenderInfo*>(data);
  auto* userData = renderInfo->userData;
  auto* scene = reinterpret_cast<Job::RenderUserData*>(userData)->scene;
  auto& world = scene->GetWorld();
  auto modelAssetMgr = AssetManager<ModelAsset>::GetInstance();

  // find all renderable model, and update the gpu mesh data
  auto modelNodeView = world.view<ModelSceneNode, RenderableTag>();
  for (auto&& [model, node] : modelNodeView.each()) {
    for (auto meshEntity : node.m_meshEntities) {
      if (!world.any_of<MeshComponent>(meshEntity)) continue;

      auto& meshComponent = world.get<MeshComponent>(meshEntity);
      auto& index = meshComponent.index;
      auto& mesh = meshComponent.m_modelAsset->GetMesh(index);

      if (!world.any_of<MeshRenderComponent>(meshEntity)) {
        scene->Emplace<MeshRenderComponent>(meshEntity, m_rhiFactory, m_sampler, m_emptyImageView);
        scene->Update<MeshRenderComponent>(meshEntity, [&](auto& component) {
          component.Init(mesh);
          return false;  // No Need to emit an update signal
        });
      }

      scene->Update<MeshRenderComponent>(meshEntity, [&](auto&& component) {
        component.Update(mesh);
        return true;
      });

      /**
       * clear flags
       */
      mesh.m_materialTexChanged = false;
      mesh.m_materialValueChanged = false;
    }
  }

  // TODO: remove unused MeshRenderComponent
}

}  // namespace Marbas::Job
