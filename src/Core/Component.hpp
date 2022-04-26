#ifndef MARBAS_CORE_COMPONENT_HPP
#define MARBAS_CORE_COMPONENT_HPP

#include "Common.hpp"
#include "Resource/ResourceManager.hpp"

namespace Marbas {

#pragma pack(1)
extern "C" struct MeshVertexInfo {
    float posX = 0.f, posY = 0.f, posZ = 0.f;
    float normalX = 0.f, normalY = 0.f, normalZ = 0.f;
    float textureU = 0.f, textureV = 0.f;
    int32_t diffuseTextureId = 0, ambientTextureId = 0;
};
#pragma pack()

struct MeshComponent {
    Vector<MeshVertexInfo> m_vertices;
    Vector<uint32_t> m_indices;
};

struct TransformComponent {
    glm::mat4 modelMatrix;
};

struct StaticMeshComponent {
};

struct RenderComponent {
    MaterialResource* m_material;
    ShaderResource* m_shader;

    std::unique_ptr<DrawBatch> m_drawBatch = nullptr;
};

}  // namespace Marbas

#endif
