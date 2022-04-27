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
    [[nodiscard]] Shader* LoadShader();

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

    bool m_isLink = false;
    bool m_isLoad = false;
};

}  // namespace Marbas

#endif
