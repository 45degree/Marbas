#ifndef MARBAS_RESOURCE_SHADER_RESOURCE_HPP
#define MARBAS_RESOURCE_SHADER_RESOURCE_HPP

#include "Resource/ResourceBase.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

class ShaderResource final : public ResourceBase {
public:
    explicit ShaderResource(std::unique_ptr<Shader>&& shader):
        ResourceBase(),
        m_shader(std::move(shader))
    {}

public:
    [[nodiscard]] Shader* GetShader() const noexcept {
        if(m_shader == nullptr) return nullptr;
        if(m_vertexShader == nullptr || m_fragmentShader == nullptr) return nullptr;
        if(m_isLink) return m_shader.get();

        m_shader->AddShaderCode(m_vertexShader.get());
        m_shader->AddShaderCode(m_fragmentShader.get());
        m_shader->Link();
        m_isLink = true;

        return m_shader.get();
    }

    void SetVertexShader(std::unique_ptr<ShaderCode>&& vertexShader) {
        m_vertexShader = std::move(vertexShader);
        m_isLink = false;
    }

    void SetFragmentShader(std::unique_ptr<ShaderCode>&& fragmentShader) {
        m_fragmentShader = std::move(fragmentShader);
        m_isLink = false;
    }

private:
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<ShaderCode> m_vertexShader;
    std::unique_ptr<ShaderCode> m_fragmentShader;
    mutable bool m_isLink = false;
};

}  // namespace Marbas

#endif
