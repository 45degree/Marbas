#ifndef MARBARS_CORE_MESH_H
#define MARBARS_CORE_MESH_H

#include "Common.h"
#include "Renderer/Interface/DrawCall.h"
#include "Renderer/Interface/Texture.h"

#include <assimp/scene.h>

namespace Marbas {

class Mesh {
public:
    Mesh() = default;
    ~Mesh() = default;

public:
    void ReadModle(const FileSystem::path& modelPath);

private:
    void ProcessMesh(const aiMesh* mesh ,const aiScene* scene);

public:
    std::unique_ptr<DrawCall> drawCall;
    Vector<std::unique_ptr<Texture*>> textures;
};

}  // namespace Marbas

#endif
