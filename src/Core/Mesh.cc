#include "Core/Mesh.hpp"
#include "RHI/RHI.hpp"
#include "Common.hpp"
#include "Tool/EncodingConvert.hpp"

#include <glog/logging.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

namespace Marbas {

void Mesh::ReadFromNode(const aiMesh* mesh, const aiScene* scene) {

    // set vertex buffer
    auto* texture = mesh->mTextureCoords[0];
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        auto vertex = mesh->mVertices[i];
        auto normal = mesh->mNormals[i];

        m_vertices.push_back(vertex.x);
        m_vertices.push_back(vertex.y);
        m_vertices.push_back(vertex.z);
        m_vertices.push_back(normal.x);
        m_vertices.push_back(normal.y);
        m_vertices.push_back(normal.z);

        if(texture == nullptr) {
            m_vertices.push_back(0);
            m_vertices.push_back(0);
        }
        else {
            m_vertices.push_back(texture[i].x);
            m_vertices.push_back(texture[i].y);
        }
    }

    // set face (index buffer)
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        auto face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++) {
            m_indices.push_back(face.mIndices[j]);
        }
    }

    // load texture path
    if (mesh->mMaterialIndex >= 0) {
        m_texturePathes.clear();
        auto* material = scene->mMaterials[mesh->mMaterialIndex];
        LoadMaterialTexturePath(material, aiTextureType_DIFFUSE);
        // LoadMaterialTexturePath(material, aiTextureType_AMBIENT);
    }
}

void Mesh::LoadToGPU(bool force) {
    if(!force && m_isLoadToGPU) {
        return;
    }

    // set element layout
    ElementLayout vertexElement, normalElement, textureCoorElement;
    vertexElement.index = 0;
    vertexElement.size = 3;
    normalElement.index = 1;
    normalElement.size = 3;
    textureCoorElement.index = 2;
    textureCoorElement.size = 2;
    Vector<ElementLayout> layout({vertexElement, normalElement, textureCoorElement});

    m_vertexBuffer = RHIFactory::GetInstance(RendererType::OPENGL)->CreateVertexBuffer(m_vertices);
    m_vertexBuffer->SetLayout(layout);

    m_indicesBuffer = RHIFactory::GetInstance(RendererType::OPENGL)->CreateIndexBuffer(m_indices);

    m_textures.clear();
    for(const auto& path : m_texturePathes) {
        auto texture = RHIFactory::GetInstance(RendererType::OPENGL)->CreateTexutre2D(path);
        m_textures.push_back(texture);
    }

    m_drawUnit.m_indicesBuffer = m_indicesBuffer.get();
    m_drawUnit.m_vertexBuffer = m_vertexBuffer.get();

    for(auto& texture : m_textures) {
        m_drawUnit.textures.push_back(texture);
    }

    LOG(INFO) << "Load the mesh to GPU";

    m_isLoadToGPU = true;
}

void Mesh::UnLoadFromGPU() {
    m_vertexBuffer.reset();
    m_indicesBuffer.reset();

    m_drawUnit.m_indicesBuffer = nullptr;
    m_drawUnit.m_vertexBuffer = nullptr;

    m_isLoadToGPU = false;
}

void Mesh::LoadMaterialTexturePath(const aiMaterial* material, aiTextureType type) {
    for(auto i = 0; i < material->GetTextureCount(type); i++) {
        aiString str;
        material->GetTexture(type, i, &str);
        auto texturePath = (m_meshPath / str.C_Str()).string();
#ifdef _WIN32
    std::replace(texturePath.begin(), texturePath.end(), '/', '\\');
#elif __linux__
    std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
#endif
        m_texturePathes.push_back(texturePath);
    }
}

}  // namespace Marbas
