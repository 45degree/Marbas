#ifndef MARBARS_LAYER_RENDER_LAYER_H
#define MARBARS_LAYER_RENDER_LAYER_H

#include "Layer/LayerBase.h"
#include "RHI/RHI.h"
#include "Core/Camera.h"
#include "Core/Model.h"
#include "Core/Scene.h"

namespace Marbas {

class RenderLayer : public LayerBase{
public:
    RenderLayer(int width, int height, const  Window* window);
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

    Camera* GetCamera() {
        return m_editorCamera.get();
    }

    void SetSecne(std::unique_ptr<Scene>&& scene) {
        m_scene = std::move(scene);
    }

    [[nodiscard]] Scene* GetScene() const {
        return m_scene.get();
    }

private:
    void RenderScenNode(SceneNode* ndoe);

private:
    std::unique_ptr<ShaderCode> vertexShader;
    std::unique_ptr<ShaderCode> fragmentShader;

    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<FrameBuffer> m_frameBuffer;
    std::unique_ptr<Viewport> m_viewport;
    std::unique_ptr<Camera> m_editorCamera;

    std::unique_ptr<Scene> m_scene;

    FrameBufferInfo m_frameBufferInfo;
    RHIFactory* m_rhiFactory;

private:
    bool m_isFrameChanged = false;
    float m_mouseLastX;
    float m_mouseLastY;
};

}  // namespace Marbas

#endif
