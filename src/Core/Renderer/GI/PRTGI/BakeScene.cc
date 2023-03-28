#include "BakeScene.hpp"

namespace Marbas::GI::PRTGI {

BakeScene::BakeScene(RHIFactory* rhiFactory) : m_rhiFactory(rhiFactory) {
  // TODO: set pre-complile result in this
  m_renderSceneFromProbe = std::make_unique<RenderSceneFromProbe>(m_rhiFactory);

  ImageCreateInfo imageCreateInfo{
      .width = 512,
      .height = 512,
      .format = ImageFormat::RGBA32F,
      .sampleCount = SampleCount::BIT1,
      .mipMapLevel = 1,
      .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
      .imageDesc = CubeMapImageDesc{},
  };
  m_posImage = m_rhiFactory->GetBufferContext()->CreateImage(imageCreateInfo);
  m_normalImage = m_rhiFactory->GetBufferContext()->CreateImage(imageCreateInfo);
  imageCreateInfo.usage = ImageUsageFlags::DEPTH_STENCIL;
  imageCreateInfo.format = ImageFormat::DEPTH;
  m_depthImage = m_rhiFactory->GetBufferContext()->CreateImage(imageCreateInfo);

  ImageViewCreateInfo imageViewCreateInfo{
      .image = m_posImage,
      .type = ImageViewType::CUBEMAP,
      .baseLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 6,
  };
  m_posImageView = m_rhiFactory->GetBufferContext()->CreateImageView(imageViewCreateInfo);

  imageViewCreateInfo.image = m_normalImage;
  m_normalImageView = m_rhiFactory->GetBufferContext()->CreateImageView(imageViewCreateInfo);

  imageViewCreateInfo.image = m_depthImage;
  m_depthImageView = m_rhiFactory->GetBufferContext()->CreateImageView(imageViewCreateInfo);
}

BakeScene::~BakeScene() {
  auto* bufCtx = m_rhiFactory->GetBufferContext();
  bufCtx->DestroyImageView(m_posImageView);
  bufCtx->DestroyImageView(m_normalImageView);
  bufCtx->DestroyImageView(m_depthImageView);

  bufCtx->DestroyImage(m_posImage);
  bufCtx->DestroyImage(m_normalImage);
  bufCtx->DestroyImage(m_depthImage);
}

void
BakeScene::Bake() {
  GenerateProbe();

  for (auto& probe : m_lightProbes) {
    auto probePos = probe.m_pos;

    m_renderSceneFromProbe->SetProbe(probePos);
    m_renderSceneFromProbe->Render(RenderSceneFromProbe::RenderInfo{
        .scene = m_scene,
        .width = m_cubemapRes,
        .height = m_cubemapRes,
        .posCubemap = m_posImageView,
        .normalCubemap = m_normalImageView,
        .depthCubemap = m_depthImageView,
    });

    // TODO: prt
    auto bufCtx = m_rhiFactory->GetBufferContext();
    ImageSubresourceDesc subResDesc;
    subResDesc.layerCount = 1;
    subResDesc.mipmapLevel = 0;

    BrickFactor brickFactor;
    Brick brick;
    for (int layer = 0; layer < 6; layer++) {
      subResDesc.baseArrayLayer = layer;
      subResDesc.image = m_normalImage;

      auto normalImageSize = bufCtx->GetImageSubresourceSize(subResDesc);
      std::vector<glm::vec4> normalImageData(std::ceil((float)normalImageSize / sizeof(glm::vec4)));
      bufCtx->GetImageData(subResDesc, normalImageData.data());

      subResDesc.image = m_posImage;
      auto posImageSize = bufCtx->GetImageSubresourceSize(subResDesc);
      std::vector<glm::vec4> posImageData(std::ceil((float)normalImageSize / sizeof(glm::vec4)));
      bufCtx->GetImageData(subResDesc, posImageData.data());

      DLOG_ASSERT(posImageData.size() == normalImageData.size())
          << "the size of normal image of a probe should be equal to"
             "the position image of the probe in the baking";

      for (int i = 0; i < posImageData.size(); i++) {
        auto pos = glm::vec3(posImageData[i]);
        auto normal = glm::vec3(normalImageData[i]);

        if (glm::length(normal) < 0.1) continue;  // TODO: sky visibility?

        glm::vec3 probeToPos = pos - probePos;
        if (glm::dot(probeToPos, normal) > 0) continue;  // TODO: back face

        Surfel surfel(pos, normal);
        if (auto iter = std::find(m_surfels.cbegin(), m_surfels.cend(), surfel); iter == m_surfels.cend()) {
          m_surfels.emplace_back(surfel);
          brick.surfelIndex.push_back(m_surfels.size() - 1);
        } else {
          brick.surfelIndex.push_back(iter - m_surfels.cbegin());
        }

        /**
         * calculate transfer weight
         */

        // calculate the solid solid
        // see https://www.rorydriscoll.com/2012/01/15/cubemap-texel-solid-angle/
        // and the comment https://www.rorydriscoll.com/2012/01/15/cubemap-texel-solid-angle/#div-comment-1258
        glm::vec3 cube_coord =
            probeToPos / std::max({std::abs(probeToPos.x), std::abs(probeToPos.y), std::abs(probeToPos.z)});
        float weight = glm::dot(cube_coord, cube_coord);
        weight *= std::sqrt(weight);
        float solid_angle = 4.0 / m_cubemapRes / m_cubemapRes / weight;

        auto d = glm::normalize(cube_coord);
        brick.transfer_weight.push_back(SphericalHarmonic(d) * solid_angle);
      }
    }
    m_bricks.push_back(brick);
    brickFactor.m_brickIndex = m_bricks.size() - 1;
    probe.m_brickFactors = {brickFactor};
  }
}

};  // namespace Marbas::GI::PRTGI
