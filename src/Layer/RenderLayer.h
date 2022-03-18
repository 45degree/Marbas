#ifndef MARBARS_LAYER_RENDER_LAYER_H
#define MARBARS_LAYER_RENDER_LAYER_H

#include "Layer/Layer.h"
#include "Renderer/Interface/VertexBuffer.h"
#include "Renderer/Interface/VertexArray.h"
#include "Renderer/Interface/IndexBuffer.h"
#include "Renderer/Interface/Viewport.h"
#include "Renderer/Interface/ShaderCode.h"
#include "Renderer/Interface/Shader.h"
#include "Renderer/Interface/FrameBuffer.h"

#include "Core/Camera.h"
#include "Core/Model.h"

namespace Marbas {

class RenderLayer : public Layer{
public:
    RenderLayer(): Layer("RenderLayer") {}
    ~RenderLayer() override;

public:
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;

public:
    [[nodiscard]] const void* GetFrameBufferTexture() const noexcept{
        return frameBuffer->GetColorAttachTexture();
    }

    [[nodiscard]] const Viewport* GetViewport() const noexcept {
        return viewport.get();
    }

    void AddViewport(std::unique_ptr<Viewport>&& viewport) {
        this->viewport = std::move(viewport);
    }

private:
    std::unique_ptr<ShaderCode> vertexShader;
    std::unique_ptr<ShaderCode> fragmentShader;
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<FrameBuffer> frameBuffer;
    std::unique_ptr<Viewport> viewport;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<Model> model;


    ////
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<VertexArray> vertexArray;
    std::unique_ptr<IndexBuffer> indexBuffer;

    std::unique_ptr<Texture2D> texture1;
    std::unique_ptr<Texture2D> texture2;

    DrawUnit drawUnit;
    OpenGLDrawCollection collection;
};

}  // namespace Marbas

#endif
