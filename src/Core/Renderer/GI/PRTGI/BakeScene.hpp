#pragma once

#include "Core/Renderer/Pass/PreComputePass/RenderSceneFromProbe.hpp"
#include "Core/Scene/Scene.hpp"
#include "RHIFactory.hpp"
#include "Surfel.hpp"

namespace Marbas::GI::PRTGI {

class BakeScene {
 public:
  BakeScene(RHIFactory* rhiFactory);
  ~BakeScene();

 public:
  void
  SetScene(Scene* scene) {
    m_scene = scene;
  }

  void
  Bake();

 private:
  void
  GenerateProbe() {
    m_lightProbes = {LightProbe{.m_pos = glm::vec3(0, 0, 0)}};
  }

 private:
  Scene* m_scene;
  RHIFactory* m_rhiFactory;

  std::unique_ptr<RenderSceneFromProbe> m_renderSceneFromProbe;
  Image* m_posImage;
  Image* m_normalImage;
  Image* m_depthImage;
  ImageView* m_posImageView;
  ImageView* m_normalImageView;
  ImageView* m_depthImageView;

  uint32_t m_cubemapRes = 512;

  Vector<LightProbe> m_lightProbes;
  Vector<BrickFactor> m_brickFactor;
  Vector<Brick> m_bricks;
  Vector<Surfel> m_surfels;
};

};  // namespace Marbas::GI::PRTGI
