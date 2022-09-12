#include "Core/Layer/RenderLayer.hpp"

#include "Common/Common.hpp"
#include "Common/EditorCamera.hpp"
#include "Core/Application.hpp"
#include "Core/Event/Input.hpp"
#include "Core/Renderer/BeginningRenderPass.hpp"
#include "Core/Renderer/BillBoardRenderPass.hpp"
#include "Core/Renderer/BlinnPhongRenderPass.hpp"
#include "Core/Renderer/CubeMapRenderPass.hpp"
#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Renderer/GridRenderPass.hpp"
#include "Core/Renderer/PointLightShadowMappingRenderPass.hpp"
#include "Core/Renderer/ShadowMappingRenderPass.hpp"
#include "RHI/RHI.hpp"

namespace Marbas {

struct RenderSceneInfo {
  glm::vec3 cameraPos = glm::vec3(0, 0, 0);
};

RenderLayer::RenderLayer(int width, int height, const std::weak_ptr<Window>& window)
    : LayerBase(window), m_width(width), m_height(height) {
  m_rhiFactory = Application::GetRendererFactory();
  m_renderGraph = std::make_shared<RenderGraph>(width, height, m_rhiFactory);
  m_allScene = {
      std::make_shared<Scene>(m_resourceManager),
  };
}

RenderLayer::~RenderLayer() = default;

void
RenderLayer::OnAttach() {
  // create beginning render target
  auto beginningDepthTarget = std::make_shared<RenderTargetNode>(RenderTargetNodeCreateInfo{
      .targetName = BeginningRenderPass::depthTargetName,
      .buffersType =
          {
              GBufferType{.type = GBufferTexutreType::DEPTH, .levels = 1, .layers = 1},
          },
      .rhiFactory = m_rhiFactory,
      .width = m_width,
      .height = m_height,
  });

  auto beginningTarget = std::make_shared<RenderTargetNode>(RenderTargetNodeCreateInfo{
      .targetName = BeginningRenderPass::targetName,
      .buffersType =
          {
              GBufferType{.type = GBufferTexutreType::COLOR, .levels = 1, .layers = 1},
          },
      .rhiFactory = m_rhiFactory,
      .width = m_width,
      .height = m_height,
  });
  m_renderGraph->RegisterRenderTargetNode(beginningDepthTarget);
  m_renderGraph->RegisterRenderTargetNode(beginningTarget);

  auto beginningRenderPass = std::make_shared<BeginningRenderPass>([&]() {
    BeginningRenderPassCreateInfo createInfo;
    createInfo.resourceManager = m_resourceManager;
    createInfo.rhiFactory = m_rhiFactory;
    createInfo.width = m_width;
    createInfo.height = m_height;
    return createInfo;
  }());
  m_renderGraph->RegisterDeferredRenderPassNode(beginningRenderPass);

  // create geometry render pass and target
  auto geometryDepthTarget = std::make_shared<RenderTargetNode>(RenderTargetNodeCreateInfo{
      .targetName = GeometryRenderPass::depthTargetName,
      .buffersType =
          {
              GBufferType{.type = GBufferTexutreType::DEPTH, .levels = 1, .layers = 1},
          },
      .rhiFactory = m_rhiFactory,
      .width = m_width,
      .height = m_height,
  });
  auto geometryTarget = std::make_shared<RenderTargetNode>(RenderTargetNodeCreateInfo{
      .targetName = GeometryRenderPass::geometryTargetName,
      .buffersType =
          {
              GBufferType{.type = GBufferTexutreType::COLOR, .levels = 1, .layers = 1},
              GBufferType{.type = GBufferTexutreType::NORMALS, .levels = 1, .layers = 1},
              GBufferType{.type = GBufferTexutreType::POSITION, .levels = 1, .layers = 1},
          },
      .rhiFactory = m_rhiFactory,
      .width = m_width,
      .height = m_height,
  });

  m_renderGraph->RegisterRenderTargetNode(geometryDepthTarget);
  m_renderGraph->RegisterRenderTargetNode(geometryTarget);

  auto geometryRenderPass = std::make_shared<GeometryRenderPass>([&]() {
    GeometryRenderPassCreatInfo createInfo;
    createInfo.resourceManager = m_resourceManager;
    createInfo.rhiFactory = m_rhiFactory;
    createInfo.width = m_width;
    createInfo.height = m_height;
    return createInfo;
  }());
  m_renderGraph->RegisterDeferredRenderPassNode(geometryRenderPass);

  // shadow mapping render pass
  auto shadowMappingTarget = std::make_shared<RenderTargetNode>(RenderTargetNodeCreateInfo{
      .targetName = String(ShadowMappingRenderPass::renderTarget),
      .buffersType =
          {
              GBufferType{
                  .type = GBufferTexutreType::SHADOW_MAP,
                  .levels = 1,
                  .layers = ShadowMappingRenderPass::MAX_LIGHT_COUNT,
              },
          },
      .rhiFactory = m_rhiFactory,
      .width = 4096,
      .height = 4096,
  });
  m_renderGraph->RegisterRenderTargetNode(shadowMappingTarget);

  auto shadowMappingRenderPass = std::make_shared<ShadowMappingRenderPass>([&]() {
    ShadowMappingCreateInfo createInfo;
    createInfo.resourceManager = m_resourceManager;
    createInfo.rhiFactory = m_rhiFactory;
    createInfo.width = 4096;
    createInfo.height = 4096;
    return createInfo;
  }());
  m_renderGraph->RegisterDeferredRenderPassNode(shadowMappingRenderPass);

  // point light shadow mapping render pass
  auto pointLightshadowMappTarget = std::make_shared<RenderTargetNode>(RenderTargetNodeCreateInfo{
      .targetName = String(PointLightShadowMappingRenderPass::targetName),
      .buffersType =
          {
              GBufferType{
                  .type = GBufferTexutreType::SHADOW_MAP_CUBE,
                  .levels = 1,
                  .layers = PointLightShadowMappingRenderPass::MAX_LIGHT_COUNT,
              },
          },
      .rhiFactory = m_rhiFactory,
      .width = 2048,
      .height = 2048,
  });
  m_renderGraph->RegisterRenderTargetNode(pointLightshadowMappTarget);

  auto pointLightShadowMapRenderPass = std::make_shared<PointLightShadowMappingRenderPass>([&]() {
    PointLightShadowMappingRenderPassCreateInfo createInfo;
    createInfo.resourceManager = m_resourceManager;
    createInfo.rhiFactory = m_rhiFactory;
    createInfo.width = 2048;
    createInfo.height = 2048;
    return createInfo;
  }());
  m_renderGraph->RegisterDeferredRenderPassNode(pointLightShadowMapRenderPass);

  // blinnPhone Render Pass
  auto blinnPhoneTarget = std::make_shared<RenderTargetNode>(RenderTargetNodeCreateInfo{
      .targetName = BlinnPhongRenderPass::blinnPhongTargetName,
      .buffersType =
          {
              GBufferType{.type = GBufferTexutreType::COLOR, .levels = 1, .layers = 1},
          },
      .rhiFactory = m_rhiFactory,
      .width = m_width,
      .height = m_height,
  });
  m_renderGraph->RegisterRenderTargetNode(blinnPhoneTarget);

  auto blinnPhoneRenderPass = std::make_shared<BlinnPhongRenderPass>([&]() {
    BlinnPhongRenderPassCreateInfo createInfo;
    createInfo.resourceManager = m_resourceManager;
    createInfo.rhiFactory = m_rhiFactory;
    createInfo.width = m_width;
    createInfo.height = m_height;
    return createInfo;
  }());
  m_renderGraph->RegisterDeferredRenderPassNode(blinnPhoneRenderPass);

  // create billBoard render pass
  auto billBoardRenderPass = std::make_shared<BillBoardRenderPass>([&]() {
    BillBoardRenderPassCreateInfo createInfo;
    createInfo.resourceManager = m_resourceManager;
    createInfo.rhiFactory = m_rhiFactory;
    createInfo.width = m_width;
    createInfo.height = m_height;
    return createInfo;
  }());
  m_renderGraph->RegisterForwardRenderPassNode(billBoardRenderPass);

  // create cube map render pass
  auto cubeMapRenderPass = std::make_shared<CubeMapRenderPass>([&]() {
    CubeMapRenderPassCreateInfo createInfo;
    createInfo.resourceManager = m_resourceManager;
    createInfo.rhiFactory = m_rhiFactory;
    createInfo.width = m_width;
    createInfo.height = m_height;
    return createInfo;
  }());
  m_renderGraph->RegisterForwardRenderPassNode(cubeMapRenderPass);

  // create grid render pass
  auto gridRenderPass = std::make_shared<GridRenderPass>([&]() {
    GridRenderPassCreateInfo createInfo;
    createInfo.resourceManager = m_resourceManager;
    createInfo.rhiFactory = m_rhiFactory;
    createInfo.width = m_width;
    createInfo.height = m_height;
    return createInfo;
  }());
  m_renderGraph->RegisterForwardRenderPassNode(gridRenderPass);

  // compile render graph
  m_renderGraph->Compile();
}

void
RenderLayer::OnDetach() {}

void
RenderLayer::OnUpdate() {
  m_renderGraph->Execute(m_resourceManager, m_allScene[m_activeSceneIndex]);
}

void
RenderLayer::OnMouseMove(const MouseMoveEvent& e) {
  auto [x, y] = e.GetPos();
  auto xOffset = x - m_mouseLastX;
  auto yOffset = y - m_mouseLastY;

  auto editorCamera = m_allScene[m_activeSceneIndex]->GetEditorCamrea();

  if (Input::IsKeyPress(GLFW_KEY_LEFT_SHIFT)) {
    if (Input::IsMousePress(GLFW_MOUSE_BUTTON_MIDDLE)) {
      // calculate current plane

      auto right = editorCamera->GetRightVector();  // x-axis in camera coordinate
      auto pos = editorCamera->GetPosition();
      auto up = editorCamera->GetUpVector();

      // The movement of the viewpoint is always opposite to the mouse, and the direction of the
      // v-axis in the image coordinate system is also opposite to the direction of the y-axis in
      // the camera coordinate, so the y-axis coordinate calculation does not need to take a
      // negative sign.
      auto moveOffset = -xOffset * right + yOffset * up;
      editorCamera->MoveFixPoint(moveOffset.x, moveOffset.y, moveOffset.z);
    }
  } else if (Input::IsMousePress(GLFW_MOUSE_BUTTON_MIDDLE)) {
    editorCamera->AddPitch(yOffset);
    editorCamera->AddYaw(-xOffset);
  }

  m_mouseLastX = x;
  m_mouseLastY = y;
}

void
RenderLayer::OnMouseScrolled(const MouseScrolledEvent& e) {
  auto yOffset = e.GetYOffset();
  auto editorCamera = m_allScene[m_activeSceneIndex]->GetEditorCamrea();
  editorCamera->AddDistance(yOffset * 10);
}

std::shared_ptr<Texture>
RenderLayer::GetRenderResult() {
  auto target = m_renderGraph->GetRenderTarget(String(BlinnPhongRenderPass::blinnPhongTargetName));

  return target->GetGBuffer()->GetTexture(GBufferTexutreType::COLOR);
}

}  // namespace Marbas
