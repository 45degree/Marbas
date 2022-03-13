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
#include "Renderer/OpenGL/OpenGLTexture.h"

namespace Marbas {

RenderLayer::~RenderLayer() = default;

void RenderLayer::OnAttach() {
    Vector<float> vertices = {
        //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
    };

    Vector<int> indices  {
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    auto _vertexBuffer = std::make_unique<OpenGLVertexBuffer>(vertices);

    ElementLayout layout1, layout2, layout3;
    layout1.index = 0;
    layout1.size = 3;
    layout2.index = 1;
    layout2.size = 3;
    layout3.index = 2;
    layout3.size = 2;

    _vertexBuffer->SetLayout({layout1, layout2, layout3});
    vertexBuffer = std::move(_vertexBuffer);

    vertexArray = std::make_unique<OpenGLVertexArray>();
    indexBuffer = std::make_unique<OpenGLIndexBuffer>(indices);

    auto _vertexShader = std::make_unique<OpenGLShader>(ShaderType::VERTEX_SHADER);
    _vertexShader->ReadSPIR_V("shader/shader.vert.spv", "main");
    auto _fragmentShader = std::make_unique<OpenGLShader>(ShaderType::FRAGMENT_SHADER);
    _fragmentShader->ReadSPIR_V("shader/shader.frag.spv", "main");

    FrameBufferInfo info;
    info.width = 800;
    info.height = 600;

    auto _frameBufer = std::make_unique<OpenGLFrameBuffer>(info);
    _frameBufer->Create();
    frameBuffer = std::move(_frameBufer);

    vertexShader = std::move(_vertexShader);
    fragmentShader = std::move(_fragmentShader);

    auto _texture = std::make_unique<OpenGLTexture2D>("resource/container.jpg");
    texture1 = std::move(_texture);

    auto _texture2 = std::make_unique<OpenGLTexture2D>("resource/awesomeface.png");
    texture2 = std::move(_texture2);

    drawCall = std::make_unique<OpenGLDrawCall>();
    drawCall->AddVertices(vertexBuffer.get(), vertexArray.get());
    drawCall->AddIndeices(indexBuffer.get());

    drawCall->AddShader(vertexShader.get());
    drawCall->AddShader(fragmentShader.get());

    drawCall->AddTexture(texture1.get(), 0);
    drawCall->AddTexture(texture2.get(), 1);

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
