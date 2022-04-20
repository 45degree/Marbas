#ifndef MARBAS_LAYER_RESOURCE_LAYER_HPP
#define MARBAS_LAYER_RESOURCE_LAYER_HPP

#include "Common.hpp"
#include "Resource/ResourceManager.hpp"
#include "Layer/LayerBase.hpp"

namespace Marbas {

class ResourceLayer : public LayerBase {
public:
    explicit ResourceLayer(const Window* window);

private:
    std::unique_ptr<ResourceManager> m_resourceManager;

};

}  // namespace Marbas

#endif
