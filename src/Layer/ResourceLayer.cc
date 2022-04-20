#include "Layer/ResourceLayer.hpp"
#include "Core/Application.hpp"

namespace Marbas {

ResourceLayer::ResourceLayer( const Window* window) :
    LayerBase(window)
{
    auto rhi = Application::GetRendererFactory();
    m_resourceManager = std::make_unique<ResourceManager>(rhi);
}

}  // namespace Marbas
