#ifndef MARBARS_LAYER_RENDER_LAYER_H
#define MARBARS_LAYER_RENDER_LAYER_H

#include "Layer/Layer.h"
#include "Renderer/RendererCommon.h"

#include "Core/Camera.h"
#include "Core/Model.h"

namespace Marbas {

class RenderLayer : public Layer{
public:
    RenderLayer();
    ~RenderLayer() override;

public:
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;

    void OnMouseMove(const MouseMoveEvent& e) override;

    void OnMouseScrolled(const MouseScrolledEvent& e) override;

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
    Vector<std::unique_ptr<Model>> models;

    RendererFactory* m_rendererFactory;

private:
    double m_mouseLastX;
    double m_mouseLastY;

};

}  // namespace Marbas

#endif
