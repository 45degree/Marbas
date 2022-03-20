#include "Layer/RenderLayer.h"
#include "Common.h"
#include "Core/Application.h"
#include "Core/Model.h"
#include "Renderer/RendererCommon.h"
#include "Event/Input.h"

namespace Marbas {

RenderLayer::RenderLayer() : Layer("RenderLayer") {
   m_rendererFactory = RendererFactory::GetInstance(RendererType::OPENGL);
}

RenderLayer::~RenderLayer() = default;

void RenderLayer::OnAttach() {

    // create framebuffer
    FrameBufferInfo info;
    info.depthAttach = true;
    info.width = 800;
    info.height = 600;

    auto _frameBufer = m_rendererFactory->CreateFrameBuffer(info);
    _frameBufer->Create();
    frameBuffer = std::move(_frameBufer);

    model = std::make_unique<Model>();
    model->ReadFromFile("resource/nanosuit/nanosuit.obj");
    model->GenerateGPUData();

    camera = std::make_unique<Camera>();
    camera->SetFixPoint(glm::vec3(0, 0, 0));

    vertexShader = m_rendererFactory->CreateShaderCode("shader/shader.vert", ShaderCodeType::FILE,
                                                       ShaderType::VERTEX_SHADER);

    fragmentShader = m_rendererFactory->CreateShaderCode("shader/shader.frag", ShaderCodeType::FILE,
                                                         ShaderType::FRAGMENT_SHADER);

    m_shader = m_rendererFactory->CreateShader();
    m_shader->AddShaderCode(fragmentShader.get());
    m_shader->AddShaderCode(vertexShader.get());
    m_shader->Link();

    auto* collection = model->GetDrawCollection();
    for(auto drawUnit : collection->m_drawUnits) {
        drawUnit->m_shader = m_shader.get();
    }
}

void RenderLayer::OnDetach() {}

void RenderLayer::OnUpdate() {
    frameBuffer->Bind();
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    viewport->UseViewport();

    auto viewMatrix = camera->GetViewMartix();
    auto projectionMatrix = camera->GetPerspective(800.0 / 600.0);
    auto modelMatrix = model->GetModelMatrix();

    struct MVP {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    } mvp;

    mvp.model = modelMatrix;
    mvp.view = viewMatrix;
    mvp.projection = projectionMatrix;

    m_shader->AddUniformDataBlock(0,  &mvp, sizeof(MVP));

    model->Draw();

    frameBuffer->UnBind();
    glDisable(GL_DEPTH_TEST);
}

void RenderLayer::OnMouseMove(const MouseMoveEvent& e) {
    auto [x, y] = e.GetPos();
    double xOffset = x - m_mouseLastX;
    double yOffset = y - m_mouseLastY;

    if(Input::IsKeyPress(GLFW_KEY_LEFT_SHIFT)) {
        if(Input::IsMousePress(GLFW_MOUSE_BUTTON_MIDDLE)) {
            camera->MoveFixPoint(-xOffset, yOffset);
        }
    }
    else if (Input::IsMousePress(GLFW_MOUSE_BUTTON_MIDDLE)) {
            camera->AddPitch(yOffset);
            camera->AddYaw(-xOffset);
    }

    m_mouseLastX = x;
    m_mouseLastY = y;
}

void RenderLayer::OnMouseScrolled(const MouseScrolledEvent& e) {
    auto yOffset = e.GetYOffset();
    camera->AddFov(yOffset);
}

}  // namespace Marbas
