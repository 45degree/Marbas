#ifndef MARBAS_RHI_INTERFACE_MATERIAL_HPP
#define MARBAS_RHI_INTERFACE_MATERIAL_HPP

#include "RHI/Interface/Texture.hpp"
#include "RHI/Interface/Shader.hpp"

namespace Marbas {

class Material {
public:
    Material() = default;
    ~Material() = default;

public:
    Vector<Texture2D*> m_textures;
    Shader* shader;
};

}  // namespace Marbas

#endif
