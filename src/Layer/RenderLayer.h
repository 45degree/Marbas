#ifndef MARBARS_LAYER_RENDER_LAYER_H
#define MARBARS_LAYER_RENDER_LAYER_H

#include "Layer/Layer.h"
#include "RHI/RHI.h"
#include "Core/Camera.h"
#include "Core/Model.h"

namespace Marbas {

class RenderLayer : public Layer{
public:
    RenderLayer(int width, int height);
    ~RenderLayer() override;

public:
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;

    void OnMouseMove(const MouseMoveEvent& e) override;

    void OnMouseScrolled(const MouseScrolledEvent& e) override;

public:
    [[nodiscard]] const void* GetFrameBufferTexture() const noexcept{
        return m_frameBuffer->GetColorAttachTexture();
    }

    void ChangeFrameSize(int width, int height) noexcept {
        if(m_frameBufferInfo.width == width && m_frameBufferInfo.height == height) return;

        m_frameBufferInfo.width = width;
        m_frameBufferInfo.height = height;
        m_isFrameChanged = true;
    }

    [[nodiscard]] const Viewport* GetViewport() const noexcept {
        return m_viewport.get();
    }

    void AddModle(std::unique_ptr<Model>&& model) {
        models.push_back(std::move(model));
    }

    Camera* GetCamera() {
        return m_editorCamera.get();
    }

private:
    std::unique_ptr<ShaderCode> vertexShader;
    std::unique_ptr<ShaderCode> fragmentShader;
    Vector<std::unique_ptr<Model>> models;

    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<FrameBuffer> m_frameBuffer;
    std::unique_ptr<Viewport> m_viewport;
    std::unique_ptr<Camera> m_editorCamera;

    FrameBufferInfo m_frameBufferInfo;
    RHIFactory* m_rhiFactory;

private:
    bool m_isFrameChanged = false;
    double m_mouseLastX;
    double m_mouseLastY;
};

}  // namespace Marbas

#endif
