#ifndef MARBAS_CORE_COMPONENT_HPP
#define MARBAS_CORE_COMPONENT_HPP

#include <unordered_map>

#include "Common.hpp"
#include "Resource/ResourceManager.hpp"

namespace Marbas {

#pragma pack(1)
extern "C" struct MeshVertexInfo {
  float posX = 0.f, posY = 0.f, posZ = 0.f;
  float normalX = 0.f, normalY = 0.f, normalZ = 0.f;
  float textureU = 0.f, textureV = 0.f;
};
#pragma pack()

enum class TagsKey {
  Name,
  EntityType,
};

enum class EntityType {
  Mesh,
  CubeMap,
  Light,
};

struct TagsCompoment {
  String name;
  EntityType type;

  std::unordered_map<TagsKey, String> tags;
};

struct MeshComponent {
  Vector<MeshVertexInfo> m_vertices;
  Vector<uint32_t> m_indices;
};

struct TransformComponent {
  glm::mat4 modelMatrix = glm::mat4(1.0f);
};

struct StaticMeshComponent {};

struct RenderComponent {
  bool m_isOnGPU = false;
  MaterialResource* m_materialResource = nullptr;
  std::unique_ptr<DrawBatch> m_drawBatch = nullptr;
};

struct CubeMapComponent {
  bool m_isOnGPU = false;
  std::unique_ptr<DrawBatch> m_drawBatch = nullptr;
  CubeMapResource* m_cubeMapResource = nullptr;
  MaterialResource* m_materialResource = nullptr;
};

}  // namespace Marbas

#endif
