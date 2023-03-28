#include "ShadowComponent.hpp"

#include "Core/Scene/Component/LightComponent.hpp"

namespace Marbas {

static std::vector<glm::vec4>
GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view) {
  const auto inv = glm::inverse(proj * view);

  std::vector<glm::vec4> frustumCorners;
  for (unsigned int x = 0; x < 2; ++x) {
    for (unsigned int y = 0; y < 2; ++y) {
      for (unsigned int z = 0; z < 2; ++z) {
        const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, z, 1.0f);
        frustumCorners.push_back(pt / pt.w);
      }
    }
  }

  return frustumCorners;
}

static glm::mat4
GetLightSpaceMatrix(const Camera* camera, const glm::vec3 lightDir, const float nearPlane, const float farPlane) {
  // glm::perspective()
  auto fov = camera->GetFov();
  auto aspect = camera->GetAspect();
  const auto proj = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
  const auto corners = GetFrustumCornersWorldSpace(proj, camera->GetViewMatrix());

  glm::vec3 center = glm::vec3(0, 0, 0);
  for (const auto& v : corners) {
    center += glm::vec3(v);
  }
  center /= corners.size();

  const auto lightView = glm::lookAt(center - lightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

  float minX = std::numeric_limits<float>::max();
  float maxX = std::numeric_limits<float>::lowest();
  float minY = std::numeric_limits<float>::max();
  float maxY = std::numeric_limits<float>::lowest();
  float minZ = std::numeric_limits<float>::max();
  float maxZ = std::numeric_limits<float>::lowest();
  for (const auto& v : corners) {
    const auto trf = lightView * v;
    minX = std::min(minX, trf.x);
    maxX = std::max(maxX, trf.x);
    minY = std::min(minY, trf.y);
    maxY = std::max(maxY, trf.y);
    minZ = std::min(minZ, trf.z);
    maxZ = std::max(maxZ, trf.z);
  }

  // Tune this parameter according to the scene
  constexpr float zMult = 10.0f;
  if (minZ < 0) {
    minZ *= zMult;
  } else {
    minZ /= zMult;
  }
  if (maxZ < 0) {
    maxZ /= zMult;
  } else {
    maxZ *= zMult;
  }

  const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

  return lightProjection * lightView;
}

template <size_t N>
static std::array<glm::mat4, N + 1>
GetLightSpaceMatrices(const Camera* camera, const glm::vec3& lightDir, const std::array<float, N>& splits) {
  float farPlane = camera->GetFar();
  float nearPlane = camera->GetNear();
  std::array<float, N> shadowCascadeLevels;
  for (int i = 0; i < N; i++) {
    shadowCascadeLevels[i] = farPlane * splits[i];
  }

  std::array<glm::mat4, N + 1> ret;
  for (size_t i = 0; i < N + 1; ++i) {
    if (i == 0) {
      ret[i] = GetLightSpaceMatrix(camera, lightDir, nearPlane, shadowCascadeLevels[i]);
    } else if (i < N) {
      ret[i] = GetLightSpaceMatrix(camera, lightDir, shadowCascadeLevels[i - 1], shadowCascadeLevels[i]);
    } else {
      ret[i] = GetLightSpaceMatrix(camera, lightDir, shadowCascadeLevels[i - 1], farPlane);
    }
  }
  return ret;
}

void
DirectionShadowComponent::Update(entt::registry& world, Camera* camera) {
  auto view = world.view<DirectionShadowComponent, DirectionLightComponent>();

  // TODO: FITTING the frustum
  //
  // auto sceneAABB = m_scene->GetSceneAABB();
  // auto center = glm::to_string(sceneAABB.GetCenter());
  // auto extent = glm::to_string(sceneAABB.GetExtent());
  // LOG(INFO) << FORMAT("scene AABB, center: {}, extent: {}", center, extent);

  for (auto&& [entity, shadowComponent, lightComponent] : view.each()) {
    auto& light = lightComponent.m_light;
    auto lightDir = light.GetDirection();
    auto lightSpaceMatrices = GetLightSpaceMatrices(camera, lightDir, shadowComponent.split);
    for (int i = 0; i < lightSpaceMatrices.size(); i++) {
      shadowComponent.lightSpaceMatrices[i] = lightSpaceMatrices[i];
    }
  }
}

}  // namespace Marbas
