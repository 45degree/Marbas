#include "Core/Mesh.h"
#include "Common.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Marbas {

void Mesh::ReadModle(const FileSystem::path &modelPath) {
    Assimp::Importer importer;
    auto filename = modelPath.string();
    const auto* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  {
        LOG(ERROR) << FORMAT("Failed to load model: {}", importer.GetErrorString());
        return;
    }
    modelPath.parent_path();
}

}  // namespace Marbas
