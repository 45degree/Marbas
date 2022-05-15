#include "Layer/RenderLayer.hpp"

#include "Common.hpp"
#include "Core/Application.hpp"
#include "Core/Entity.hpp"
#include "Core/Mesh.hpp"
#include "Event/Input.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

struct RenderSceneInfo {
  glm::vec3 cameraPos = glm::vec3(0, 0, 0);
};

RenderLayer::RenderLayer(int width, int height, ResourceManager* resourceManager,
                         const Window* window)
    : LayerBase(window), m_resourceManager(resourceManager) {
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

  m_scene = std::make_unique<Scene>("", m_resourceManager);
}

RenderLayer::~RenderLayer() = default;

void RenderLayer::OnAttach() {}

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

  RenderSceneInfo sceneInfo;
  sceneInfo.cameraPos = m_editorCamera->GetPosition();

  // TODO: draw static batch

  auto& registry = m_scene->GetRigister();

  auto view = registry.view<TransformComponent, RenderComponent>();
  view.each([&](auto entity, TransformComponent& transform, RenderComponent& render) {
    MVP mvp;
    mvp.projection = projectionMatrix;
    mvp.view = viewMatrix;

    if (!render.m_isOnGPU) {
      MeshPolicy::LoadToGPU(entity, m_scene.get(), m_rhiFactory, m_resourceManager);
      render.m_isOnGPU = true;
    }

    auto* material = render.m_drawBatch->GetMaterial();
    auto* shader = material->GetShader();
    mvp.model = transform.modelMatrix;
    shader->AddUniformDataBlock(0, &mvp, sizeof(MVP));
    shader->AddUniformDataBlock(1, &sceneInfo, sizeof(RenderSceneInfo));

    CubeMap cubeMap = m_scene->GetSkyBox();
    if (Entity::HasComponent<CubeMapComponent>(m_scene.get(), cubeMap)) {
      auto& component = Entity::GetComponent<CubeMapComponent>(m_scene.get(), cubeMap);
      const auto& cubeMapResource = component.m_cubeMapResource;
      if (!cubeMapResource->IsLoad()) {
        cubeMapResource->LoadResource(m_rhiFactory);
      }
      render.m_drawBatch->SetCubeMapTexture(cubeMapResource->GetCubeMapTexture());
    }

    render.m_drawBatch->Draw();
  });

  auto cubeMap = registry.view<MeshComponent, CubeMapComponent>();
  cubeMap.each([&](auto entity, MeshComponent& mesh, CubeMapComponent& cubeMapComponent) {
    if (!cubeMapComponent.m_isOnGPU) {
      if (cubeMapComponent.m_cubeMapResource == nullptr) return;

      CubeMapPolicy::LoadToGPU(entity, m_scene.get(), m_rhiFactory, m_resourceManager);
      cubeMapComponent.m_isOnGPU = true;
    }

    MVP mvp;
    mvp.projection = projectionMatrix;
    mvp.view = glm::mat4(glm::mat3(viewMatrix));
    mvp.model = glm::mat4(1.0);

    auto* material = cubeMapComponent.m_drawBatch->GetMaterial();
    auto* shader = material->GetShader();
    shader->AddUniformDataBlock(0, &mvp, sizeof(MVP));

    cubeMapComponent.m_drawBatch->SetDepthFunc(DepthFunc::LEQUAL);
    cubeMapComponent.m_drawBatch->Draw();
  });

  m_frameBuffer->UnBind();
  m_rhiFactory->Disable(EnableItem::DEPTH);
}

void RenderLayer::OnMouseMove(const MouseMoveEvent& e) {
  auto [x, y] = e.GetPos();
  auto xOffset = x - m_mouseLastX;
  auto yOffset = y - m_mouseLastY;

  if (Input::IsKeyPress(GLFW_KEY_LEFT_SHIFT)) {
    if (Input::IsMousePress(GLFW_MOUSE_BUTTON_MIDDLE)) {
      m_editorCamera->MoveFixPoint(-xOffset, yOffset);
    }
  } else if (Input::IsMousePress(GLFW_MOUSE_BUTTON_MIDDLE)) {
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
