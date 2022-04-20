#ifndef MARBAS_RESOURCE_SHADER_RESOURCE_HPP
#define MARBAS_RESOURCE_SHADER_RESOURCE_HPP

#include "Resource/ResourceBase.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

class ShaderResource final : public ResourceBase {
public:
    ShaderResource() : ResourceBase(ResourceType::SHADER) {}

public:
    [[nodiscard]] Shader* GetShader() const noexcept {
        if(m_shader == nullptr) return nullptr;

        return m_shader.get();
    }

private:
    std::unique_ptr<Shader> m_shader;
};

}  // namespace Marbas

#endif
