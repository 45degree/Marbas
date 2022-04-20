#ifndef MARBAS_RESOURCE_TEXTURE_RESOURCE_HPP
#define MARBAS_RESOURCE_TEXTURE_RESOURCE_HPP

#include "Resource/ResourceBase.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

class TextureResource : public ResourceBase {
protected:
    explicit TextureResource(ResourceType type, int id = 0) : ResourceBase(type, id) {}
};

class Texture2DResource final : public TextureResource {
public:
    explicit Texture2DResource(std::unique_ptr<Texture2D>&& texture, int id = 0) :
        TextureResource(ResourceType::TEXTURE2D, id),
        m_texture(std::move(texture))
    {}

public:
    [[nodiscard]] Texture2D* GetTexture() const noexcept {
        if(m_texture == nullptr) return nullptr;
        return m_texture.get();
    }

private:
    std::unique_ptr<Texture2D> m_texture;
};

// TODO: 3d texture

}  // namespace Marbas

#endif
