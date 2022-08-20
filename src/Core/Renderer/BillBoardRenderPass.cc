#include "Core/Renderer/BillBoardRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Renderer/BlinnPhongRenderPass.hpp"
#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Scene/Component/BillBoardComponent.hpp"
#include "Core/Scene/Entity/Entity.hpp"

namespace Marbas {

struct BillBoardComponent_Impl {
  std::shared_ptr<VertexBuffer> vertexBuffer;
  std::shared_ptr<DescriptorSet> descriptorSet;
  std::shared_ptr<Texture2DResource> textureResource;
};

static Vector<ElementLayout>
GetMeshVertexInfoLayout() {
  Vector<ElementLayout> layouts{
      ElementLayout{0, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
      ElementLayout{1, ElementType::FLOAT, sizeof(float), 3, false, 0, 0},
      ElementLayout{2, ElementType::FLOAT, sizeof(float), 2, false, 0, 0},
  };

  ElementLayout::CalculateLayout(layouts);

  return layouts;
};

BillBoardRenderPassCreateInfo::BillBoardRenderPassCreateInfo() {
  passName = "CubeMapRenderPass";
  inputPassNode = BlinnPhongRenderPass::renderPassName;
}

BillBoardRenderPass::BillBoardRenderPass(const BillBoardRenderPassCreateInfo& createInfo)
    : ForwardRenderPass(createInfo) {
  DLOG_ASSERT(m_rhiFactory != nullptr)
      << FORMAT("can't Initialize the {}, because the rhiFactory isn't been set",
                NAMEOF_TYPE(BillBoardRenderPass));

  DLOG_ASSERT(m_resourceManager != nullptr)
      << FORMAT("can't Initialize the {}, because the resource manager isn't been set",
                NAMEOF_TYPE(BillBoardRenderPass));

  /**
   * set render pass and pipeline
   */

  // create render pass
  RenderPassCreateInfo renderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
                  // result attachment
                  .format = TextureFormat::RGBA,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
              AttachmentDescription{
                  // input depth attachment from geometryRenderPass's output
                  .format = TextureFormat::DEPTH,
                  .type = AttachmentType::Depth,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
          },
  };
  m_renderPass = m_rhiFactory->CreateRenderPass(renderPassCreateInfo);

  // create command factory
  m_commandFactory = m_rhiFactory->CreateCommandFactory();
  m_commandBuffer = m_commandFactory->CreateCommandBuffer();

  // read shader
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/billBoard.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/billBoard.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);

  // create Pipeline
  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetVertexBufferLayout(GetMeshVertexInfoLayout(), VertexInputRate::INSTANCE);
  m_pipeline->SetVertexInputBindingDivisor({{0, 1}});
  m_pipeline->SetShader(shaderResource->GetShader());
  m_pipeline->SetDepthStencilInfo(DepthStencilInfo{.depthTestEnable = true});
  m_pipeline->Create();

  // create uniform buffer
  auto matrixBufferSize = sizeof(BillBoardRenderPass::MatrixUniformBufferBlock);
  auto cameraBufferSize = sizeof(BillBoardRenderPass::CameraInfoUniformBufferBlock);
  m_matrixUniformBuffer = m_rhiFactory->CreateUniformBuffer(matrixBufferSize);
  m_cameraUniformBuffer = m_rhiFactory->CreateUniformBuffer(cameraBufferSize);
}

void
BillBoardRenderPass::CreateBufferForEveryEntity(const entt::entity entity, const Scene* scene) {
  DLOG_ASSERT(Entity::HasComponent<BillBoardComponent>(scene, entity));

  auto& billBoardComponent =
      Entity::GetComponent<BillBoardComponent>(const_cast<Scene*>(scene), entity);
  if (billBoardComponent.implData != nullptr) return;

  auto implData = std::make_shared<BillBoardComponent_Impl>();

  // create vertex buffer and index buffer
  struct VertexData {
    glm::vec3 point = glm::vec3(0, 0, 0);
    glm::vec3 axis = glm::vec3(0, 1, 0);
    glm::vec2 size = glm::vec2(10, 10);
  } vertexData;

  auto verticesLen = sizeof(vertexData);
  auto vertexBuffer = m_rhiFactory->CreateVertexBuffer(&vertexData, verticesLen);
  vertexBuffer->SetLayout(GetMeshVertexInfoLayout());
  implData->vertexBuffer = std::move(vertexBuffer);

  // load material
  if (billBoardComponent.textureResourceId.has_value()) {
    auto id = billBoardComponent.textureResourceId.value();
    auto texture2DResource = m_resourceManager->GetTexture2DResourceContainer()->GetResource(id);
    texture2DResource->LoadResource(m_rhiFactory, m_resourceManager.get());

    DescriptorSetInfo descriptorSetInfo{
        DescriptorInfo{
            .isBuffer = true,
            .type = BufferDescriptorType::UNIFORM_BUFFER,
            .bindingPoint = 0,
        },
        DescriptorInfo{
            .isBuffer = true,
            .type = BufferDescriptorType::UNIFORM_BUFFER,
            .bindingPoint = 1,
        },
        DescriptorInfo{
            .isBuffer = false,
            .bindingPoint = 0,
        },
    };
    auto descriptor = m_rhiFactory->CreateDescriptorSet(descriptorSetInfo);
    descriptor->BindImage(0, texture2DResource->GetTexture()->GetDescriptor());
    descriptor->BindBuffer(0, m_matrixUniformBuffer->GetIBufferDescriptor());
    descriptor->BindBuffer(1, m_cameraUniformBuffer->GetIBufferDescriptor());

    implData->descriptorSet = std::move(descriptor);
    implData->textureResource = texture2DResource;
  }
  billBoardComponent.implData = implData;
  m_needToRecordComand = true;
}

void
BillBoardRenderPass::RecordCommand(const Scene* scene) {
  m_commandBuffer->Clear();
  auto view = Entity::GetAllEntity<BillBoardComponent>(scene);

  // check framebuffer and renderpass
  DLOG_ASSERT(m_framebuffer != nullptr)
      << FORMAT("{}'s framebuffer is null, can't record command", NAMEOF_TYPE(BillBoardRenderPass));

  DLOG_ASSERT(m_renderPass != nullptr)
      << FORMAT("{}'s render pass is null, can't record command", NAMEOF_TYPE(BillBoardRenderPass));

  DLOG_ASSERT(m_pipeline != nullptr)
      << FORMAT("{}'s pipeline is null, can't record command", NAMEOF_TYPE(BillBoardRenderPass));

  // recreate uniform buffer

  /**
   * set command
   */
  auto beginRenderPass = m_commandFactory->CreateBeginRenderPassCMD();
  beginRenderPass->SetRenderPass(m_renderPass);
  beginRenderPass->SetFrameBuffer(m_framebuffer);
  beginRenderPass->SetClearColor({0, 0, 0, 1});

  auto endRenderPass = m_commandFactory->CreateEndRenderPassCMD();
  endRenderPass->SetRenderPass(m_renderPass);
  endRenderPass->SetFrameBuffer(m_framebuffer);

  // set bind pipeline
  auto bindPipeline = m_commandFactory->CreateBindPipelineCMD();
  bindPipeline->SetPipeLine(m_pipeline);

  /**
   * begin to record command
   */

  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->AddCommand(std::move(beginRenderPass));
  m_commandBuffer->AddCommand(std::move(bindPipeline));

  for (const auto& [entity, component] : view.each()) {
    const auto& billBoardComponent = Entity::GetComponent<BillBoardComponent>(scene, entity);
    const auto& implData = billBoardComponent.implData;
    DLOG_ASSERT(implData->vertexBuffer != nullptr);
    auto bindVertexBuffer = m_commandFactory->CreateBindVertexBufferCMD();
    auto bindDescriptorSet = m_commandFactory->CreateBindDescriptorSetCMD();
    auto drawArray = m_commandFactory->CreateDrawArrayCMD(m_pipeline);
    bindVertexBuffer->SetVertexBuffer(implData->vertexBuffer);
    bindDescriptorSet->SetDescriptor(implData->descriptorSet);
    drawArray->SetVertexCount(6);
    drawArray->SetInstanceCount(1);
    m_commandBuffer->AddCommand(std::move(bindVertexBuffer));
    m_commandBuffer->AddCommand(std::move(bindDescriptorSet));
    m_commandBuffer->AddCommand(std::move(drawArray));
  }

  m_commandBuffer->AddCommand(std::move(endRenderPass));
  m_commandBuffer->EndRecordCmd();
}

void
BillBoardRenderPass::SetUniformBuffer(const Scene* scene) {
  // set matrix
  const auto editorCamera = scene->GetEditorCamrea();
  const auto viewMatrix = editorCamera->GetViewMartix();
  const auto perspectiveMatrix = editorCamera->GetPerspective();

  m_matrixUniformBlock.view = viewMatrix;
  m_matrixUniformBlock.perspective = perspectiveMatrix;
  m_matrixUniformBuffer->SetData(&m_matrixUniformBlock, sizeof(MatrixUniformBufferBlock), 0);

  // set camera info
  auto right = editorCamera->GetRightVector();
  auto up = editorCamera->GetUpVector();
  m_cameraUniformBlock.right = right;
  m_cameraUniformBlock.up = up;
  m_cameraUniformBuffer->SetData(&m_cameraUniformBlock, sizeof(CameraInfoUniformBufferBlock), 0);
}

void
BillBoardRenderPass::Execute(const Scene* scene, const ResourceManager* resourceManager) {
  auto view = Entity::GetAllEntity<BillBoardComponent>(scene);
  for (auto&& [entity, cubeMapComponent] : view.each()) {
    CreateBufferForEveryEntity(entity, scene);
  }

  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }

  SetUniformBuffer(scene);
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
