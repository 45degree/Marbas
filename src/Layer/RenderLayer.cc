#include "Layer/RenderLayer.h"
#include "Common.h"
#include "Core/Application.h"
#include "Core/Model.h"
#include "Renderer/OpenGL/OpenGLVertexBuffer.h"
#include "Renderer/Interface/VertexBuffer.h"
#include "Renderer/OpenGL/OpenGLVertexArray.h"
#include "Renderer/OpenGL/OpenGLIndexBuffer.h"
#include "Renderer/OpenGL/OpenGLShaderCode.h"
#include "Renderer/OpenGL/OpenGLShader.h"
#include "Renderer/OpenGL/OpenGLViewport.h"
#include "Renderer/OpenGL/OpenGLFrameBuffer.h"
#include "Renderer/OpenGL/OpenGLTexture.h"

namespace Marbas {

RenderLayer::~RenderLayer() = default;

void RenderLayer::OnAttach() {

    // create framebuffer
    FrameBufferInfo info;
    info.width = 800;
    info.height = 600;

    auto _frameBufer = std::make_unique<OpenGLFrameBuffer>(info);
    _frameBufer->Create();
    frameBuffer = std::move(_frameBufer);

    glEnable(GL_DEPTH_TEST);

    model = std::make_unique<Model>();
    model->ReadFromFile("resource/nanosuit/nanosuit.obj");
    model->GenerateGPUData();

    camera = std::make_unique<Camera>();
    camera->SetPosition(glm::vec3(0, 0, 20));

    auto _vertexShader = std::make_unique<OpenGLShaderCode>(ShaderType::VERTEX_SHADER);
    _vertexShader->ReadFromSource("shader/shader.vert");
    auto _fragmentShader = std::make_unique<OpenGLShaderCode>(ShaderType::FRAGMENT_SHADER);
    _fragmentShader->ReadFromSource("shader/shader.frag");

    vertexShader = std::move(_vertexShader);
    fragmentShader = std::move(_fragmentShader);

    m_shader = std::make_unique<OpenGLShader>();
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

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    viewport->UseViewport();

    auto viewMatrix = camera->GetViewMartix();
    auto projectionMatrix = camera->GetPerspective(glm::radians(45.0f), 800.0 / 600.0);
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
}

}  // namespace Marbas
