#ifndef MARBAS_CORE_COMPONENT_HPP
#define MARBAS_CORE_COMPONENT_HPP

#include "Common.hpp"
#include "Resource/ResourceManager.hpp"

#include <unordered_map>

namespace Marbas {

#pragma pack(1)
extern "C" struct MeshVertexInfo {
    float posX = 0.f, posY = 0.f, posZ = 0.f;
    float normalX = 0.f, normalY = 0.f, normalZ = 0.f;
    float textureU = 0.f, textureV = 0.f;
};
#pragma pack()

enum class TagsKey {
    NAME
};

struct TagsCompoment {
    std::unordered_map<TagsKey, String> tags;
};

struct MeshComponent {
    Vector<MeshVertexInfo> m_vertices;
    Vector<uint32_t> m_indices;
};

struct TransformComponent {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
};

struct StaticMeshComponent {
};

struct RenderComponent {
    bool m_isOnGPU = false;
    std::optional<Uid> m_materialResource = std::nullopt;
    std::unique_ptr<DrawBatch> m_drawBatch = nullptr;
};

}  // namespace Marbas

#endif
