#include "Renderer/OpenGL/OpenGLShaderCode.h"

#include <fstream>

namespace Marbas {

static GLenum ConvertToOpenGLShaderType(const ShaderType& type) noexcept {
    switch(type) {
        case ShaderType::VERTEX_SHADER: return GL_VERTEX_SHADER;
        case ShaderType::FRAGMENT_SHADER: return GL_FRAGMENT_SHADER;
    }
}

void OpenGLShaderCode::ReadSPIR_V(const FileSystem::path& path, const String& enterPoint) {
    std::ifstream file;
    file.open(path, std::ios_base::in | std::ios_base::binary);
    if(!file.is_open()) return;

    // read the spri-v content
    Vector<char> content;
    try {
        file.seekg(0, std::ios::beg);
        file.seekg(0, std::ios::end);
        auto size = file.tellg();
        content.resize(size);
        file.seekg(0, std::ios::beg);
        file.read(content.data(), size);
    }
    catch(const std::exception& e) {
        file.close();
        throw e;
    }

    auto contentSize = static_cast<GLsizei>(content.size());
    glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, content.data(), contentSize);
    glSpecializeShader(shaderID, enterPoint.c_str(), 0, nullptr, nullptr);

    // Specialization is equivalent to compilation.
    GLint isCompiled = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

        LOG(INFO) << infoLog.data();

        // We don't need the shader anymore.
        glDeleteShader(shaderID);

        // Use the infoLog as you see fit.

        // In this simple program, we'll just leave
        return;
    }
}

void OpenGLShaderCode::ReadFromSource(const FileSystem::path& path) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    // read source file
    String content;
    try {
        file.open(path);
        file.seekg(0, std::ios::beg);

        std::stringstream stringStream;
        stringStream << file.rdbuf();
        file.close();

        content = stringStream.str();
    }
    catch(const std::exception& e) {
        LOG(ERROR) << e.what();

        if(file.is_open()) {
            file.close();
        }

        throw e;
    }

    const char* shaderSource = content.c_str();
    glShaderSource(shaderID, 1, &shaderSource, nullptr);
    glCompileShader(shaderID);

    GLint success = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shaderID, 1024, nullptr, infoLog);
        LOG(ERROR) << "failed to compile shader, info: \n" << infoLog;
    }
    else LOG(INFO) << "compiled shader";
}

OpenGLShaderCode::OpenGLShaderCode(const ShaderType& shaderType) : ShaderCode(shaderType) {
    GLenum openglShaderType = ConvertToOpenGLShaderType(shaderType);
    shaderID = glCreateShader(openglShaderType);
}

OpenGLShaderCode::~OpenGLShaderCode() {
    glDeleteShader(shaderID);
}

} // namespace Marbas
