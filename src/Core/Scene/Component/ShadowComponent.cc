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

  auto viewDir = camera->GetFrontVector();
  const auto lightView = glm::lookAt(center - lightDir, center, glm::normalize(glm::cross(viewDir, lightDir)));

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
  // constexpr float zMult = 2.f;
  // if (minZ < 0) {
  //   minZ *= zMult;
  // } else {
  //   minZ /= zMult;
  // }
  // if (maxZ < 0) {
  //   maxZ /= zMult;
  // } else {
  //   maxZ *= zMult;
  // }

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

DirectionShadowComponent::DirectionShadowComponent() {
  m_shadowGPUInfo.m_argument.Bind(0, DescriptorType::UNIFORM_BUFFER);
}

void
DirectionShadowComponent::UpdateShadowGPUInfo(RHIFactory* rhiFactory, const glm::vec3& lightDir, const Camera& camera) {
  float farPlane = camera.GetFar();
  float nearPlane = camera.GetNear();

  std::array<float, splitCount> cascadeFarPlane;
  for (int i = 0; i < splitCount; i++) {
    cascadeFarPlane[i] = nearPlane + (farPlane - nearPlane) * m_split[i];
  }

  for (size_t i = 0; i < m_lightSpaceMatrices.size(); i++) {
    glm::mat4 ret;
    if (i == 0) {
      ret = GetLightSpaceMatrix(&camera, lightDir, nearPlane, cascadeFarPlane[i]);
    } else if (i < m_lightSpaceMatrices.size() - 1) {
      ret = GetLightSpaceMatrix(&camera, lightDir, cascadeFarPlane[i - 1], cascadeFarPlane[i]);
    } else {
      ret = GetLightSpaceMatrix(&camera, lightDir, cascadeFarPlane[i - 1], farPlane);
    }
    m_lightSpaceMatrices[i] = ret;
  }

  // Change GPU Buffer
  auto bufCtx = rhiFactory->GetBufferContext();
  size_t bufferSize = sizeof(glm::mat4) * m_lightSpaceMatrices.size();
  auto* bufferData = m_lightSpaceMatrices.data();
  auto& buffer = m_shadowGPUInfo.m_lightMatricesBuffer;
  if (buffer == nullptr) {
    buffer = bufCtx->CreateBuffer(BufferType::UNIFORM_BUFFER, bufferData, bufferSize, false);
  } else {
    bufCtx->UpdateBuffer(buffer, bufferData, bufferSize, 0);
  }

  if (m_shadowGPUInfo.m_descriptorSet == 0) {
    auto pipelineCtx = rhiFactory->GetPipelineContext();
    m_shadowGPUInfo.m_descriptorSet = pipelineCtx->CreateDescriptorSet(m_shadowGPUInfo.m_argument);
    pipelineCtx->BindBuffer(BindBufferInfo{
        .descriptorSet = m_shadowGPUInfo.m_descriptorSet,
        .descriptorType = DescriptorType::UNIFORM_BUFFER,
        .bindingPoint = 0,
        .buffer = m_shadowGPUInfo.m_lightMatricesBuffer,
        .offset = 0,
        .arrayElement = 0,
    });
  }

  return;
}

}  // namespace Marbas
