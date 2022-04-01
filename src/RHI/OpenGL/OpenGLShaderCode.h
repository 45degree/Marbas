#ifndef MARBAS_RHI_OPENGL_SHADERCODE_H
#define MARBAS_RHI_OPENGL_SHADERCODE_H

#include "RHI/OpenGL/OpenGLRHICommon.h"
#include "RHI/Interface/ShaderCode.h"

namespace Marbas {

class OpenGLShaderCode : public ShaderCode {
public:
    explicit OpenGLShaderCode(const ShaderType& shaderType);
    ~OpenGLShaderCode() override;

public:
    void ReadSPIR_V(const FileSystem::path& path, const String& enterPoint) override;

    void ReadFromSource(const FileSystem::path& path);

    [[nodiscard]] GLuint GetShaderID() const noexcept { return shaderID; }

private:
    GLuint shaderID;
};

}  // namespace Marbas

#endif
