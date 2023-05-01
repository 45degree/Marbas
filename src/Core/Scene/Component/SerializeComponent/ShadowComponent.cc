#include "ShadowComponent.hpp"

#include <glog/logging.h>

#include <Common/MathCommon.hpp>

#include "LightComponent.hpp"
#include "TagComponent.hpp"

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
GetLightSpaceMatrix(const Camera* camera, const glm::vec3 lightDir, const glm::vec3& upDir, const float nearPlane,
                    const float farPlane) {
  auto fov = camera->GetFov();
  auto aspect = camera->GetAspect();
  const auto proj = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
  const auto corners = GetFrustumCornersWorldSpace(proj, camera->GetViewMatrix());

  glm::vec3 center = glm::vec3(0, 0, 0);
  for (const auto& v : corners) {
    center += glm::vec3(v);
  }
  center /= corners.size();

  const auto lightView = glm::lookAt(center - lightDir, center, upDir);

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
  constexpr float zMult = 2.f;
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

DirectionShadowComponent::DirectionShadowComponent() {}

void
DirectionShadowComponent::UpdateShadowInfo(const glm::vec3& lightDir, const Camera& camera) {
  float farPlane = camera.GetFar();
  float nearPlane = camera.GetNear();

  for (int i = 0; i < splitCount; i++) {
    m_cascadePlane[i] = nearPlane + (farPlane - nearPlane) * m_split[i];
  }
  m_cascadePlane[splitCount] = farPlane;

  auto viewDir = camera.GetFrontVector();
  auto upDir = glm::normalize(glm::cross(viewDir, lightDir));
  for (size_t i = 0; i < m_lightSpaceMatrices.size(); i++) {
    glm::mat4 ret;
    if (i == 0) {
      ret = GetLightSpaceMatrix(&camera, lightDir, upDir, nearPlane, m_cascadePlane[i]);
    } else if (i < m_lightSpaceMatrices.size() - 1) {
      ret = GetLightSpaceMatrix(&camera, lightDir, upDir, m_cascadePlane[i - 1], m_cascadePlane[i]);
    } else {
      ret = GetLightSpaceMatrix(&camera, lightDir, upDir, m_cascadePlane[i - 1], farPlane);
    }
    m_lightSpaceMatrices[i] = ret;
  }

  return;
}

void
DirectionShadowComponent::OnUpdate(entt::registry& world, entt::entity node) {
  if (!world.any_of<UpdateLightTag>(node)) {
    world.emplace<UpdateLightTag>(node);
  }
}

void
DirectionShadowComponent::OnCreate(entt::registry& world, entt::entity node) {
  if (!world.any_of<NewLightTag>(node)) {
    world.emplace<NewLightTag>(node);
  }
}

void
DirectionShadowComponent::OnDestroy(entt::registry& world, entt::entity node) {}

}  // namespace Marbas
