#ifndef MARBAS_LAYER_RESOURCE_LAYER_HPP
#define MARBAS_LAYER_RESOURCE_LAYER_HPP

#include "Common.hpp"
#include "Resource/ResourceManager.hpp"
#include "Layer/LayerBase.hpp"

namespace Marbas {

class ResourceLayer : public LayerBase {
public:
    ResourceLayer(std::unique_ptr<ResourceManager>&& resourceManager, const Window* window);

    [[nodiscard]] ResourceManager* GetResourceManager() const noexcept {
        return m_resourceManager.get();
    }

private:
    std::unique_ptr<ResourceManager> m_resourceManager;
};

}  // namespace Marbas

#endif
