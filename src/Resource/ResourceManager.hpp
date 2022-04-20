#ifndef MARBAS_RESOURCE_RESOURCE_MANAGER_HPP
#define MARBAS_RESOURCE_RESOURCE_MANAGER_HPP

#include "Common.hpp"
#include "Resource/ResourceBase.hpp"
#include "Resource/ShaderResource.hpp"
#include "Resource/TextureResource.hpp"

#include <concepts>

namespace Marbas {

class ResourceManager {
public:
    explicit ResourceManager(RHIFactory* rhiFactory) :
        m_rhiFactory(rhiFactory)
    {}

    ~ResourceManager() = default;

public:
    Texture2DResource* AddTexture(const Path& imagePath);

    ShaderResource* AddShader() {return nullptr;}

    Material* AddMaterial() {return nullptr;}

public:

    // template<typename T = ResourceBase, typename std::enable_if<std::is_base_of_v<ResourceBase, T>>>
    template<std::derived_from<ResourceBase> T>
    T* FindResource(int id) {
        auto* resource = m_resources[id].get();
        T* result = dynamic_cast<T*>(resource);
        return result;
    }

private:
    RHIFactory* m_rhiFactory;
    Vector<std::unique_ptr<ResourceBase>> m_resources;
    std::unordered_map<String, ResourceBase*> m_resourcePath;
};

}  // namespace Marbas

#endif
