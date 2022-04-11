#ifndef MARBAS_RHI_INTERFACE_MATERIAL_HPP
#define MARBAS_RHI_INTERFACE_MATERIAL_HPP

#include "RHI/Interface/Texture.hpp"
#include "RHI/Interface/Shader.hpp"

#include <unordered_set>

namespace Marbas {

/**
 * @brief the blend method, see https://learnopengl.com/Advanced-OpenGL/Blending
 */
enum class BlendFactor {
    ZERO,
    ONE,
    SRC_COLOR,
    ONE_MINUS_SRC_COLOR,
    DST_COLOR,
    ONE_MINUS_DST_COLOR,
    SRC_ALPHA,
    ONE_MINUS_SRC_ALPHA,
    DST_ALPHA,
    ONE_MINUS_DST_ALPHA,
    CONSTANT_COLOR,
    ONE_MINUS_CONSTANT_COLOR,
    CONSTANT_ALPHA,
    ONE_MINUS_CONSTANT_ALPHA,
};

class Material {
public:
    Material() = default;
    ~Material() = default;

public:
    void AddAmbientTextures(Texture2D* texture) {
        if(m_ambientTextures.find(texture) != m_ambientTextures.end()) return;

        m_ambientTextures.insert(texture);
    }

    void AddDiffuseTextures(Texture2D* texture) {
        if(m_diffuseTextures.find(texture) != m_diffuseTextures.end()) return;

        m_diffuseTextures.insert(texture);
    }

    void SetShader(Shader* shader) {
        m_shader = shader;
    }

    void SetBlendFactor(BlendFactor srcBlendType, BlendFactor dstBlendType) {
        m_srcBlendFactor = srcBlendType;
    }

    Vector<Texture2D*> GetAmbientTextures() const {
        return Vector<Texture2D*>(m_ambientTextures.begin(), m_ambientTextures.end());
    }

    Vector<Texture2D*> GetDiffuseTextures() const {
        return Vector<Texture2D*>(m_diffuseTextures.begin(), m_diffuseTextures.end());
    }

    Shader* GetShader() const noexcept {
        return m_shader;
    }

    std::tuple<BlendFactor, BlendFactor> GetBlendFactor() const noexcept {
        return {m_srcBlendFactor, m_dstBlendFactor};
    }

private:
    std::unordered_set<Texture2D*> m_ambientTextures;
    std::unordered_set<Texture2D*> m_diffuseTextures;
    BlendFactor m_srcBlendFactor = BlendFactor::SRC_ALPHA;
    BlendFactor m_dstBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
    Shader* m_shader;
};

}  // namespace Marbas

#endif
