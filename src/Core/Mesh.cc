#include "Core/Mesh.h"
#include "Renderer/OpenGL/OpenGLVertexBuffer.h"
#include "Renderer/OpenGL/OpenGLIndexBuffer.h"
#include "Renderer/OpenGL/OpenGLVertexArray.h"
#include "Common.h"

#include <assimp/Importer.hpp>
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

void Mesh::ProcessMesh(const aiMesh* mesh ,const aiScene* scene) {
    Vector<float> vertices;

    // set element layout
    ElementLayout vertexElement, normalElement, textureCoorElement;
    vertexElement.index = 0;
    vertexElement.size = 3;
    normalElement.index = 1;
    vertexElement.size = 3;
    textureCoorElement.index = 2;
    textureCoorElement.size = 2;
    Vector<ElementLayout> layout({vertexElement, normalElement, textureCoorElement});

    // set vertex buffer
    auto* texture = mesh->mTextureCoords[0];
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        auto vertex = mesh->mVertices[i];
        auto normal = mesh->mNormals[i];

        vertices.push_back(vertex.x);
        vertices.push_back(vertex.y);
        vertices.push_back(vertex.z);
        vertices.push_back(normal.x);
        vertices.push_back(normal.y);
        vertices.push_back(normal.z);

        if(texture == nullptr) {
            vertices.push_back(0);
            vertices.push_back(0);
        }
        else {
            vertices.push_back(texture[i].x);
            vertices.push_back(texture[i].y);
        }
    }

    auto vertexBuffer = std::make_unique<OpenGLVertexBuffer>(vertices);
    auto vertexArray = std::make_unique<OpenGLVertexArray>();
    vertexArray->EnableVertexAttribArray(vertexBuffer.get());

    // set face (index buffer)
    Vector<int> indices;

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        auto face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    auto indexBuffer = std::make_unique<OpenGLIndexBuffer>(indices);

    // load texture
    if (mesh->mMaterialIndex >= 0) {

    }
}

}  // namespace Marbas
