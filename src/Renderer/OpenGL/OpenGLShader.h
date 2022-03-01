#ifndef MARBARS_RENDERER_OPENGL_SHADER_H
#define MARBARS_RENDERER_OPENGL_SHADER_H

#include "Renderer/Interface/Shader.h"

namespace Marbas {

class OpenGLShader : public Shader {
public:
    explicit OpenGLShader(const ShaderType& shaderType);
    ~OpenGLShader() override;

public:
    void ReadSPIR_V(const FileSystem::path& path, const String& enterPoint) override;

    void ReadFromSource(const FileSystem::path& path);

    [[nodiscard]] GLuint GetShaderID() const noexcept { return shaderID; }

private:
    GLuint shaderID;
};

}  // namespace Marbas

#endif
