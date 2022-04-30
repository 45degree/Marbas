#ifndef MARBAS_CORE_MESH_H
#define MARBAS_CORE_MESH_H

#include "Common.hpp"
#include "RHI/RHI.hpp"
#include "Core/Component.hpp"
#include "entt/entt.hpp"

#include <assimp/scene.h>

namespace Marbas {

extern Vector<ElementLayout> GetMeshVertexInfoLayout();

using Mesh = entt::entity;

class Scene;
class MeshPolicy {
public:
    MeshPolicy() = delete;
    MeshPolicy(const MeshPolicy&) = delete;
    MeshPolicy& operator=(const MeshPolicy&) = delete;

    static Mesh Create(entt::registry& registry) {
        auto mesh = registry.create();
        registry.emplace<MeshComponent>(mesh);
        registry.emplace<RenderComponent>(mesh);
        registry.emplace<TransformComponent>(mesh);
        registry.emplace<TagsCompoment>(mesh);
        return mesh;
    }

    static void ReadVertexFromNode(const aiMesh* aMesh, const aiScene* aScene,
                                   MeshComponent& meshComponent);

    static void ReadMaterialFromNode(const aiMesh* aMesh, const aiScene* aScene, const Path& path,
                                     RenderComponent& renderComponent, 
                                     ResourceManager* resourceManager);

    static void LoadMeshToGPU(Mesh mesh, Scene* scene, RHIFactory* rhiFactory,
                              ResourceManager* resourceManager);
};

}  // namespace Marbas

#endif
