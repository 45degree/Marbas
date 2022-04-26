#include "Resource/ResourceManager.hpp"

namespace Marbas{

Texture2DResource* ResourceManager::AddTexture(const Path& imagePath) {
    const auto pathString = imagePath.string();
    if(m_staticResourcePath.find(pathString) != m_staticResourcePath.end()) {
        auto uid = m_staticResourcePath.at(pathString);
        const auto& resouce = m_resources.at(uid);
        return dynamic_cast<Texture2DResource*>(resouce.get());
    }

    auto texture = m_rhiFactory->CreateTexutre2D(imagePath);
    auto texture2DResource = std::make_unique<Texture2DResource>(std::move(texture));
    auto* texture2DResourcePtr = texture2DResource.get();
    auto uid = texture2DResource->GetUid();

    m_staticResourcePath[pathString] = uid;
    m_resources.insert({uid, std::move(texture2DResource)});

    return texture2DResourcePtr;
}

ShaderResource* ResourceManager::AddShader(const ShaderFileInfo& shaderFileInfo) {

    auto vertexShaderCode = m_rhiFactory->CreateShaderCode(shaderFileInfo.fragmentShaderPath,
                                                           shaderFileInfo.type, 
                                                           ShaderType::VERTEX_SHADER);

    auto fragmentShaderCode = m_rhiFactory->CreateShaderCode(shaderFileInfo.fragmentShaderPath,
                                                              shaderFileInfo.type,
                                                              ShaderType::FRAGMENT_SHADER);

    auto shader = m_rhiFactory->CreateShader();

    auto shaderResource = std::make_unique<ShaderResource>(std::move(shader));
    shaderResource->SetVertexShader(std::move(vertexShaderCode));
    shaderResource->SetFragmentShader(std::move(fragmentShaderCode));

    auto uid = shaderResource->GetUid();
    auto shaderResourcePtr = shaderResource.get();

    m_resources.insert({uid, std::move(shaderResource)});
    return shaderResourcePtr;
}

MaterialResource* ResourceManager::AddMaterial() {

    auto material = std::make_unique<Material>();
    auto materialResource = std::make_unique<MaterialResource>(std::move(material));
    auto materialResourcePtr = materialResource.get();
    auto uid = materialResource->GetUid();
    m_resources.insert({uid, std::move(materialResource)});

    return materialResourcePtr;
}

void ResourceManager::RemoveResource(const Uid& uid) {
}

}  // namespace Marbas
