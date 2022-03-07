#ifndef MARBARS_RENDERER_INTERFACE_TEXTURE_H
#define MARBARS_RENDERER_INTERFACE_TEXTURE_H

#include "Common.h"

namespace Marbas {

class Texture {
public:
    explicit Texture(const String& textureName) : textureName(textureName) {}
    virtual ~Texture() = default;

public:
    virtual void Bind(int uniformBind) = 0;
    virtual void UnBind() = 0;

    [[nodiscard]] const String& GetTextureName() const noexcept {
        return textureName;
    }

protected:
    const String textureName;
};

class Texture2D : public Texture{
public:
    explicit Texture2D(const String& textureName) : Texture(textureName) {}
    ~Texture2D() override = default;

    virtual void ReadImage(const FileSystem::path& path) = 0;
    void Bind(int uniformBind) override = 0;
    void UnBind() override = 0;

};

}  // namespace Marbas

#endif
