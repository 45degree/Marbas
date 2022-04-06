#include "Layer/RenderLayer.h"
#include "Common.h"
#include "Core/Application.h"
#include "Core/Model.h"
#include "RHI/RHI.h"
#include "Event/Input.h"

namespace Marbas {

RenderLayer::RenderLayer(int width, int height, const Window* window):
    LayerBase(window)
{
   m_rhiFactory = Application::GetRendererFactory();

   m_frameBufferInfo.height = height;
   m_frameBufferInfo.width = width;
   m_frameBufferInfo.depthAttach = true;
   m_frameBufferInfo.templateAttach = false;

   m_viewport = m_rhiFactory->CreateViewport();
   m_viewport->SetViewport(0, 0, width, height);

   m_frameBuffer = m_rhiFactory->CreateFrameBuffer(m_frameBufferInfo);
   m_frameBuffer->Create();

   m_editorCamera = std::make_unique<Camera>();
   m_editorCamera->SetFixPoint(glm::vec3(0, 0, 0));
}

RenderLayer::~RenderLayer() = default;

void RenderLayer::OnAttach() {

    // model->GenerateGPUData();

    // models.push_back(std::move(model));

    vertexShader = m_rhiFactory->CreateShaderCode("shader/shader.vert", ShaderCodeType::FILE,
                                                   ShaderType::VERTEX_SHADER);

    fragmentShader = m_rhiFactory->CreateShaderCode("shader/shader.frag", ShaderCodeType::FILE,
                                                    ShaderType::FRAGMENT_SHADER);

    m_shader = m_rhiFactory->CreateShader();
    m_shader->AddShaderCode(fragmentShader.get());
    m_shader->AddShaderCode(vertexShader.get());
    m_shader->Link();

}

void RenderLayer::OnDetach() {}

void RenderLayer::OnUpdate() {

    for(auto& _model : models) {
        auto* collection = _model->GetDrawCollection();
        for(auto drawUnit : collection->m_drawUnits) {
            drawUnit->m_shader = m_shader.get();
        }
    }

    m_frameBuffer->Bind();

    m_rhiFactory->Enable(EnableItem::DEPTH);

    m_rhiFactory->ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_rhiFactory->ClearBuffer(ClearBuferBit::COLOR_BUFFER);
    m_rhiFactory->ClearBuffer(ClearBuferBit::DEPTH_BUFFER);

    m_viewport->UseViewport();
    auto [x, y, w, h] = m_viewport->GetViewport();

    auto viewMatrix = m_editorCamera->GetViewMartix();

    auto projectionMatrix = m_editorCamera->GetPerspective();

    struct MVP {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };
    for(auto& model : models) {
        auto modelMatrix = model->GetModelMatrix();

        MVP mvp;

        mvp.model = modelMatrix;
        mvp.view = viewMatrix;
        mvp.projection = projectionMatrix;

        m_shader->AddUniformDataBlock(0,  &mvp, sizeof(MVP));

        model->Draw();
    }

    m_frameBuffer->UnBind();
    m_rhiFactory->Disable(EnableItem::DEPTH);
}

void RenderLayer::OnMouseMove(const MouseMoveEvent& e) {
    auto [x, y] = e.GetPos();
    auto xOffset = x - m_mouseLastX;
    auto yOffset = y - m_mouseLastY;

    if(Input::IsKeyPress(GLFW_KEY_LEFT_SHIFT)) {
        if(Input::IsMousePress(GLFW_MOUSE_BUTTON_MIDDLE)) {
            m_editorCamera->MoveFixPoint(-xOffset, yOffset);
        }
    }
    else if (Input::IsMousePress(GLFW_MOUSE_BUTTON_MIDDLE)) {
        m_editorCamera->AddPitch(yOffset);
        m_editorCamera->AddYaw(-xOffset);
    }

    m_mouseLastX = x;
    m_mouseLastY = y;
}

void RenderLayer::OnMouseScrolled(const MouseScrolledEvent& e) {
    auto yOffset = e.GetYOffset();
    m_editorCamera->AddFov(yOffset);
}

}  // namespace Marbas
