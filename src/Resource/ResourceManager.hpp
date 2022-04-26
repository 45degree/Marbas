#ifndef MARBAS_RESOURCE_RESOURCE_MANAGER_HPP
#define MARBAS_RESOURCE_RESOURCE_MANAGER_HPP

#include "Common.hpp"
#include "Resource/ResourceBase.hpp"
#include "Resource/ShaderResource.hpp"
#include "Resource/TextureResource.hpp"
#include "Resource/MaterialResource.hpp"

#include <concepts>

namespace Marbas {

struct ShaderFileInfo {
    ShaderCodeType type;
    Path vertexShaderPath;
    Path fragmentShaderPath;
};


class ResourceManager {
public:
    explicit ResourceManager(RHIFactory* rhiFactory) :
        m_rhiFactory(rhiFactory)
    {}

    ~ResourceManager() = default;

public:
    Texture2DResource* AddTexture(const Path& imagePath);

    ShaderResource* AddShader(const ShaderFileInfo& shaderFileInfo);

    MaterialResource* AddMaterial();

    void RemoveResource(const Uid& id);

private:
    RHIFactory* m_rhiFactory;

    std::unordered_map<Uid, std::unique_ptr<ResourceBase>> m_resources;
    std::unordered_set<Texture2DResource*> m_texture2DResources;
    std::unordered_set<MaterialResource*> m_materialResources;
    std::unordered_set<ShaderResource*> m_shaderResources;

    std::unordered_map<String, Uid> m_staticResourcePath;
};

}  // namespace Marbas

#endif
