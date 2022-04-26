#ifndef MARBAS_RESOURCE_MATERIAL_RESOURCE_HPP
#define MARBAS_RESOURCE_MATERIAL_RESOURCE_HPP

#include "Resource/ResourceBase.hpp"
#include "Resource/TextureResource.hpp"
#include "Resource/ShaderResource.hpp"

namespace Marbas {

class MaterialResource final : public ResourceBase {
public:
    explicit MaterialResource(std::unique_ptr<Material>&& material):
        ResourceBase(),
        m_material(std::move(material))
    {}

public:
    void SetShader(ShaderResource* shaderResource) {
        auto* shader = shaderResource->GetShader();
        m_material->SetShader(shader);
    }

    void SetAmbientTexture(Texture2DResource* texture2DResource) {
        auto* texture = texture2DResource->GetTexture();
        m_material->SetAmbientTexture(texture);
    }

    void SetDiffuseTexture(Texture2DResource* texture2DResource) {
        auto* texture = texture2DResource->GetTexture();
        m_material->SetDiffuseTexture(texture);
    }

private:
    std::unique_ptr<Material> m_material;
};

}  // namespace Marbas

#endif
