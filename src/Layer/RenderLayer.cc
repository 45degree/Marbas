#include "Layer/RenderLayer.h"
#include "Common.h"
#include "Core/Application.h"
#include "Renderer/OpenGL/OpenGLVertexBuffer.h"
#include "Renderer/Interface/VertexBuffer.h"
#include "Renderer/OpenGL/OpenGLVertexArray.h"
#include "Renderer/OpenGL/OpenGLIndexBuffer.h"
#include "Renderer/OpenGL/OpenGLShader.h"
#include "Renderer/OpenGL/OpenGLDrawCall.h"
#include "Renderer/OpenGL/OpenGLViewport.h"
#include "Renderer/OpenGL/OpenGLFrameBuffer.h"

namespace Marbas {

RenderLayer::~RenderLayer() = default;

void RenderLayer::OnAttach() {
    Vector<float> vertices = {
        0.5f, 0.5f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f, 0.0f   // 左上角
    };

    Vector<int> indices  {
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    auto _vertexBuffer = std::make_unique<OpenGLVertexBuffer>(vertices);

    ElementLayout layout;
    layout.index = 0;
    layout.size = 3;
    _vertexBuffer->SetLayout({layout});
    vertexBuffer = std::move(_vertexBuffer);

    vertexArray = std::make_unique<OpenGLVertexArray>();
    indexBuffer = std::make_unique<OpenGLIndexBuffer>(indices);

    auto _vertexShader = std::make_unique<OpenGLShader>(ShaderType::VERTEX_SHADER);
    _vertexShader->ReadSPIR_V("shader/shader.vert.spv", "main");
    auto _fragmentShader = std::make_unique<OpenGLShader>(ShaderType::FRAGMENT_SHADER);
    _fragmentShader->ReadSPIR_V("shader/shader.frag.spv", "main");

    FrameBufferInfo info; // {.width = 800, .height = 600};
    info.width = 800;
    info.height = 600;

    auto _frameBufer = std::make_unique<OpenGLFrameBuffer>(info);
    _frameBufer->Create();
    frameBuffer = std::move(_frameBufer);

    vertexShader = std::move(_vertexShader);
    fragmentShader = std::move(_fragmentShader);

    drawCall = std::make_unique<OpenGLDrawCall>();
    drawCall->AddVertices(vertexBuffer.get(), vertexArray.get());
    drawCall->AddIndeices(indexBuffer.get());

    drawCall->AddShader(vertexShader.get());
    drawCall->AddShader(fragmentShader.get());

    drawCall->Link();
}

void RenderLayer::OnDetach() {}

void RenderLayer::OnUpdate() {
    frameBuffer->Bind();

    glClear(GL_COLOR_BUFFER_BIT);
    viewport->UseViewport();
    drawCall->Use();
    drawCall->Draw();

    frameBuffer->UnBind();
}

}  // namespace Marbas
