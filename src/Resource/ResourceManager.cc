#include "Resource/ResourceManager.hpp"
#include <wingdi.h>

namespace Marbas{

Texture2DResource* ResourceManager::AddTexture(const Path& imagePath) {
    const auto pathString = imagePath.string();
    if(m_resourcePath.find(pathString) != m_resourcePath.end()) {
        auto resouce = m_resourcePath.at(pathString);
        return dynamic_cast<Texture2DResource*>(resouce);
    }

    auto texture = m_rhiFactory->CreateTexutre2D(imagePath);
    auto texture2DResource = std::make_unique<Texture2DResource>(std::move(texture),
                                                                 m_resources.size());
    auto* texture2DResourcePtr = texture2DResource.get();

    m_resourcePath[pathString] = texture2DResource.get();
    m_resources.push_back(std::move(texture2DResource));

    return texture2DResourcePtr;
}

}  // namespace Marbas
