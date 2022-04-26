#include "Layer/ResourceLayer.hpp"
#include "Core/Application.hpp"

namespace Marbas {

ResourceLayer::ResourceLayer(std::unique_ptr<ResourceManager>&& resourceManager,
                             const Window* window):
    LayerBase(window),
    m_resourceManager(std::move(resourceManager))
{
}

}  // namespace Marbas
