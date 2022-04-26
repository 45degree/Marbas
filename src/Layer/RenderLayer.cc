#include "Layer/RenderLayer.hpp"
#include "Common.hpp"
#include "Core/Application.hpp"
#include "RHI/RHI.hpp"
#include "Event/Input.hpp"

namespace Marbas {

RenderLayer::RenderLayer(int width, int height, ResourceManager* resourceManager,
                         const Window* window):
    LayerBase(window),
    m_resourceManager(resourceManager)
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

    vertexShader = m_rhiFactory->CreateShaderCode("shader/shader.vert.glsl", ShaderCodeType::FILE,
                                                   ShaderType::VERTEX_SHADER);

    fragmentShader = m_rhiFactory->CreateShaderCode("shader/shader.frag.glsl", ShaderCodeType::FILE,
                                                    ShaderType::FRAGMENT_SHADER);

    m_shader = m_rhiFactory->CreateShader();
    m_shader->AddShaderCode(fragmentShader.get());
    m_shader->AddShaderCode(vertexShader.get());
    m_shader->Link();
}

void RenderLayer::OnDetach() {}

void RenderLayer::OnUpdate() {

    m_frameBuffer->Bind();

    m_rhiFactory->Enable(EnableItem::DEPTH);

    m_rhiFactory->ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    m_rhiFactory->ClearBuffer(ClearBuferBit::COLOR_BUFFER);
    m_rhiFactory->ClearBuffer(ClearBuferBit::DEPTH_BUFFER);

    m_viewport->UseViewport();
    auto [x, y, w, h] = m_viewport->GetViewport();

    auto viewMatrix = m_editorCamera->GetViewMartix();
    auto projectionMatrix = m_editorCamera->GetPerspective();

    MVP mvp;
    mvp.projection = projectionMatrix;
    mvp.view = viewMatrix;

    if(m_scene != nullptr) {
        // TODO: draw static batch

        // draw scene node
        auto rootScene = m_scene->GetRootSceneNode();
        if(rootScene != nullptr) {
            RenderScenNode(rootScene, m_shader.get(), mvp);
        }
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

void RenderLayer::RenderScenNode(const SceneNode* node, Shader* shader, MVP& mvp) {
    if(node == nullptr) return;

    if(!node->IsStatic() && node->GetMeshCount() != 0) {
        auto drawBatches = node->GetDrawBatches();
        auto materials = node->GetMaterials();
        mvp.model = node->GetModelMatrix();
        shader->AddUniformDataBlock(0, &mvp, sizeof(MVP));

        for(auto* material : materials) {
            material->SetShader(shader);
        }

        for(auto* drawBatch : drawBatches) {
            drawBatch->Draw();
        }

    }

    auto subNodes = node->GetSubSceneNodes();
    for(const auto* subNode : subNodes) {
        RenderScenNode(subNode, shader, mvp);
    }
}

}  // namespace Marbas
