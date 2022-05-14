#ifndef MARBAS_RHI_INTERFACE_MATERIAL_HPP
#define MARBAS_RHI_INTERFACE_MATERIAL_HPP

#include <unordered_set>

#include "RHI/Interface/Shader.hpp"
#include "RHI/Interface/Texture.hpp"

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

struct BlendFactorInfo {
  BlendFactor srcBlendType;
  BlendFactor dstBlendType;
};

class Material {
 public:
  Material() = default;
  ~Material() = default;

 public:
  void SetAmbientTexture(Texture2D* texture) {
    if (texture == nullptr) return;

    m_ambientTexture = texture;
    m_allTextures.push_back(texture);
  }

  void SetDiffuseTexture(Texture2D* texture) {
    if (texture == nullptr) return;

    m_diffuseTexture = texture;
    m_allTextures.push_back(texture);
  }

  [[nodiscard]] Texture2D* GetAmbientTexture() const { return m_ambientTexture; }

  [[nodiscard]] Texture2D* GetDiffuseTexture() const { return m_diffuseTexture; }

  [[nodiscard]] Vector<Texture2D*> GetAllTextures() const { return m_allTextures; }

  int GetTextureBindPoint(const Texture2D* texture) {
    if (texture == nullptr) return -1;

    auto iter = std::find(m_allTextures.begin(), m_allTextures.end(), texture);
    if (iter == m_allTextures.end()) return -1;

    return static_cast<int>(std::distance(m_allTextures.begin(), iter));
  }

  void Bind() {
    if (m_diffuseTexture != nullptr) {
      m_diffuseTexture->Bind(1);
    }

    if (m_ambientTexture != nullptr) {
      m_ambientTexture->Bind(2);
    }
  }

  void UnBind() {
    if (m_diffuseTexture != nullptr) {
      m_diffuseTexture->UnBind();
    }

    if (m_ambientTexture != nullptr) {
      m_ambientTexture->UnBind();
    }
  }

  Shader* GetShader() { return m_shader; }

  void SetShader(Shader* shader) { m_shader = shader; }

 private:
  Vector<Texture2D*> m_allTextures;
  Texture2D* m_ambientTexture;
  Texture2D* m_diffuseTexture;

  Shader* m_shader = nullptr;
};

}  // namespace Marbas

#endif
