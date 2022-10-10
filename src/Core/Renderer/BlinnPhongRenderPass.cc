#include "Core/Renderer/BlinnPhongRenderPass.hpp"

#include <nameof.hpp>

#include "Core/Renderer/GeometryRenderPass.hpp"
#include "Core/Renderer/IBL_BRDF_RenderPass.hpp"
#include "Core/Renderer/IrradianceRenderPass.hpp"
#include "Core/Renderer/PointLightShadowMapRenderPass.hpp"
#include "Core/Renderer/PrefilterRenderPass.hpp"
#include "Core/Scene/Component/LightComponent.hpp"
#include "RHI/Interface/Pipeline.hpp"

namespace Marbas {

const String BlinnPhongRenderPass::renderPassName = "BlinnPhoneRenderPass";
const String BlinnPhongRenderPass::blinnPhongTargetName = "BlinnPhongTarget";

BlinnPhongRenderPassCreateInfo::BlinnPhongRenderPassCreateInfo() : DeferredRenderPassCreateInfo() {
  passName = BlinnPhongRenderPass::renderPassName;
  inputResource = {
      GeometryRenderPass::geometryTargetName,
      String(PointLightShadowMapRenderPass::targetName),
      String(DirectionLightShadowMapRenderPass::renderTarget),
      String(IrradianceRenderPass::targetName),
      String(PrefilterRenderPass::targetName),
      String(IBLBRDFRenderPass::targetName),
  };
  outputResource = {BlinnPhongRenderPass::blinnPhongTargetName,
                    GeometryRenderPass::depthTargetName};
}

BlinnPhongRenderPass::BlinnPhongRenderPass(const BlinnPhongRenderPassCreateInfo& createInfo)
    : DeferredRenderPass(createInfo) {}

void
BlinnPhongRenderPass::CreateRenderPass() {
  RenderPassCreateInfo renderPassCreateInfo{
      .attachments =
          {
              AttachmentDescription{
                  .format = TextureFormat::RGBA,
                  .type = AttachmentType::Color,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
              AttachmentDescription{
                  .format = TextureFormat::DEPTH,
                  .type = AttachmentType::Depth,
                  .loadOp = AttachmentLoadOp::Ignore,
              },
          },
  };
  m_renderPass = m_rhiFactory->CreateRenderPass(renderPassCreateInfo);
}

void
BlinnPhongRenderPass::CreateShader() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto shaderResource = shaderContainer->CreateResource();
  shaderResource->SetShaderStage(ShaderType::VERTEX_SHADER, "Shader/blinnPhong.vert.glsl");
  shaderResource->SetShaderStage(ShaderType::FRAGMENT_SHADER, "Shader/blinnPhong.frag.glsl");
  shaderResource->LoadResource(m_rhiFactory, m_resourceManager.get());
  m_shaderId = shaderContainer->AddResource(shaderResource);
}

void
BlinnPhongRenderPass::CreatePipeline() {
  auto shaderContainer = m_resourceManager->GetShaderResourceContainer();
  auto resource = shaderContainer->GetResource(m_shaderId);
  if (!resource->IsLoad()) {
    resource->LoadResource(m_rhiFactory, m_resourceManager.get());
  }

  m_pipeline = m_rhiFactory->CreateGraphicsPipeLine();
  m_pipeline->SetViewPort(ViewportInfo{.x = 0, .y = 0, .width = m_width, .height = m_height});
  m_pipeline->SetShader(resource->GetShader());
  m_pipeline->SetVertexBufferLayout({}, VertexInputRate::INSTANCE);
  m_pipeline->SetVertexInputBindingDivisor({{0, 1}});
  m_pipeline->SetDepthStencilInfo(DepthStencilInfo{
      .depthTestEnable = false,
  });
  m_pipeline->Create();
}

void
BlinnPhongRenderPass::CreateDescriptorSetLayout() {
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // base color
      .isBuffer = false,
      .bindingPoint = 0,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // normal
      .isBuffer = false,
      .bindingPoint = 1,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // position
      .isBuffer = false,
      .bindingPoint = 2,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // point light shadow
      .isBuffer = false,
      .bindingPoint = 3,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // direction light shadow
      .isBuffer = false,
      .bindingPoint = 4,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // roughness
      .isBuffer = false,
      .bindingPoint = 5,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // metallic
      .isBuffer = false,
      .bindingPoint = 6,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // ao
      .isBuffer = false,
      .bindingPoint = 7,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // irradiance
      .isBuffer = false,
      .bindingPoint = 8,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // prefilter map
      .isBuffer = false,
      .bindingPoint = 9,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // ibl brdf lod
      .isBuffer = false,
      .bindingPoint = 10,
  });

  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // direction light info
      .isBuffer = true,
      .bindingPoint = 1,
  });
  AddDescriptorSetLayoutBinding(DescriptorSetLayoutBinding{
      // point light info
      .isBuffer = true,
      .bindingPoint = 2,
  });
}

void
BlinnPhongRenderPass::OnInit() {
  m_vertexBuffer = m_rhiFactory->CreateVertexBuffer(0);
  m_descriptorSet = m_rhiFactory->CreateDescriptorSet(m_descriptorSetLayout);
  m_descriptorSet->BindBuffer(0, m_cameraUniformBuffer);

  auto directionLightBufferSize = sizeof(DirectionLightBlock);
  m_dirLightUbo = m_rhiFactory->CreateUniformBuffer(directionLightBufferSize);
  m_descriptorSet->BindBuffer(1, m_dirLightUbo);

  auto pointLightBufferSize = sizeof(PointLightInfoBlock);
  m_pointLightUniformBuffer = m_rhiFactory->CreateUniformBuffer(pointLightBufferSize);
  m_descriptorSet->BindBuffer(2, m_pointLightUniformBuffer);

  const auto& irradianceResource = m_inputTarget[String(IrradianceRenderPass::targetName)];
  auto irradianceBuffer = irradianceResource->GetGBuffer(GBufferTexutreType::HDR_IMAGE);

  const auto& prefilterResource = m_inputTarget[String(PrefilterRenderPass::targetName)];
  auto prefilterBuffer = prefilterResource->GetGBuffer(GBufferTexutreType::PRE_FILTER_CUBEMAP);

  const auto& iblBRDFResource = m_inputTarget[String(IBLBRDFRenderPass::targetName)];
  auto iblBRDFBuffer = iblBRDFResource->GetGBuffer(GBufferTexutreType::IBL_BRDF_LOD);

  // set input as image descriptor set
  auto gBuffer = m_inputTarget[GeometryRenderPass::geometryTargetName];
  auto gColorBuffer = gBuffer->GetGBuffer(GBufferTexutreType::COLOR);
  auto gNormalBuffer = gBuffer->GetGBuffer(GBufferTexutreType::NORMALS);
  auto gPositionBuffer = gBuffer->GetGBuffer(GBufferTexutreType::POSITION);
  auto gRoughnessBuffer = gBuffer->GetGBuffer(GBufferTexutreType::ROUGHTNESS);
  auto gMetallic = gBuffer->GetGBuffer(GBufferTexutreType::METALLIC);
  auto gAO = gBuffer->GetGBuffer(GBufferTexutreType::AMBIENT_OCCLUSION);

  auto pointShadowGBuffer = m_inputTarget[String(PointLightShadowMapRenderPass::targetName)];
  auto pointShadowBuffer = pointShadowGBuffer->GetGBuffer(GBufferTexutreType::SHADOW_MAP_CUBE);

  auto dirShadowGBuffer = m_inputTarget[String(DirectionLightShadowMapRenderPass::renderTarget)];
  auto dirShadowBuffer = dirShadowGBuffer->GetGBuffer(GBufferTexutreType::SHADOW_MAP);

  m_descriptorSet->BindImage(0, gColorBuffer);
  m_descriptorSet->BindImage(1, gNormalBuffer);
  m_descriptorSet->BindImage(2, gPositionBuffer);
  m_descriptorSet->BindImage(3, pointShadowBuffer);
  m_descriptorSet->BindImage(4, dirShadowBuffer);
  m_descriptorSet->BindImage(5, gRoughnessBuffer);
  m_descriptorSet->BindImage(6, gMetallic);
  m_descriptorSet->BindImage(7, gAO);
  m_descriptorSet->BindImage(8, irradianceBuffer);
  m_descriptorSet->BindImage(9, prefilterBuffer);
  m_descriptorSet->BindImage(10, iblBRDFBuffer);
}

void
BlinnPhongRenderPass::RecordCommand(const Scene* scene) {
  m_commandBuffer->Clear();

  // check framebuffer and renderpass
  DLOG_ASSERT(m_framebuffer != nullptr);
  DLOG_ASSERT(m_renderPass != nullptr);
  DLOG_ASSERT(m_pipeline != nullptr);

  // recreate uniform buffer

  auto bufferSize = sizeof(BlinnPhongRenderPass::DirectionLightBlock);
  m_dirLightUbo->SetData(&m_dirLightUboBlock, bufferSize, 0);

  /**
   * set command
   */

  m_commandBuffer->BeginRecordCmd();
  m_commandBuffer->BeginRenderPass(BeginRenderPassInfo{
      .renderPass = m_renderPass,
      .frameBuffer = m_framebuffer,
      .clearColor = {0, 0, 0, 1},
  });
  m_commandBuffer->BindPipeline(m_pipeline);
  m_commandBuffer->BindVertexBuffer(m_vertexBuffer);
  m_commandBuffer->BindDescriptorSet(BindDescriptorSetInfo{
      .descriptorSet = m_descriptorSet,
      .layouts = m_descriptorSetLayout,
  });
  m_commandBuffer->DrawArray(6, 1);
  m_commandBuffer->EndRenderPass();
  m_commandBuffer->EndRecordCmd();
}

void
BlinnPhongRenderPass::CreateFrameBuffer() {
  const auto& targetGBuffer = m_outputTarget[blinnPhongTargetName];
  auto colorBuffer = targetGBuffer->GetGBuffer(GBufferTexutreType::COLOR);

  const auto& depthGBuffer = m_outputTarget[GeometryRenderPass::depthTargetName];
  auto depthBuffer = depthGBuffer->GetGBuffer(GBufferTexutreType::DEPTH);

  if (colorBuffer == nullptr) {
    LOG(ERROR) << "can't get normal buffer or color buffer from the gbuffer";
    throw std::runtime_error("normal buffer and color buffer is needed by geometry render pass");
  }

  auto width = colorBuffer->GetWidth();
  auto height = colorBuffer->GetHeight();

  std::shared_ptr colorBufferView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = colorBuffer,
      .m_format = colorBuffer->GetFormat(),
      .m_type = colorBuffer->GetTextureType(),
      .m_layerBase = 0,
      .m_layerCount = 1,
      .m_levelBase = 0,
      .m_levelCount = 1,
  });
  std::shared_ptr depthBufferView = m_rhiFactory->CreateImageView(ImageViewDesc{
      .m_texture = depthBuffer,
      .m_format = depthBuffer->GetFormat(),
      .m_type = depthBuffer->GetTextureType(),
      .m_layerBase = 0,
      .m_layerCount = 1,
      .m_levelBase = 0,
      .m_levelCount = 1,
  });

  FrameBufferInfo createInfo{
      .width = width,
      .height = height,
      .renderPass = m_renderPass.get(),
      .attachments = {colorBufferView, depthBufferView},
  };

  m_framebuffer = m_rhiFactory->CreateFrameBuffer(createInfo);
}

void
BlinnPhongRenderPass::SetUniformBuffer(const Scene* scene) {
  auto paralleLightView = Entity::GetAllEntity<ParallelLightComponent>(scene);
  auto pointLightView = Entity::GetAllEntity<PointLightComponent>(scene);

  // set direction light
  constexpr int dirLightMaxCount = DirectionLightShadowMapRenderPass::MAX_LIGHT_COUNT;
  constexpr size_t dirLightBufSize = sizeof(DirectionLightBlock);
  const int directionLightCount = paralleLightView.size();
  m_dirLightUboBlock.lightCount = std::min(directionLightCount, dirLightMaxCount);
  for (int i = 0; i < directionLightCount; i++) {
    if (i >= dirLightMaxCount) {
      LOG(WARNING) << FORMAT("the max count of light is {}", dirLightMaxCount);
      break;
    }
    const auto entity = paralleLightView[i];
    const auto& lightComponent = paralleLightView.get<const ParallelLightComponent>(entity);
    auto pos = lightComponent.m_light.GetPos();
    auto color = lightComponent.m_light.GetColor();
    auto farPlane = lightComponent.m_light.GetFarPlane();
    m_dirLightUboBlock.lights[i].pos = pos;
    m_dirLightUboBlock.lights[i].color = color;
    m_dirLightUboBlock.lights[i].direction = lightComponent.m_light.GetDirection();
    m_dirLightUboBlock.lights[i].view = lightComponent.m_light.GetViewMatrix();
    m_dirLightUboBlock.lights[i].projection = lightComponent.m_light.GetProjectionMatrix();
  }
  m_dirLightUbo->SetData(&m_dirLightUboBlock, dirLightBufSize, 0);

  // set point light
  constexpr int pointLightMaxCount = PointLightShadowMapRenderPass::MAX_LIGHT_COUNT;
  constexpr size_t pointLightBufSize = sizeof(PointLightInfoBlock);
  const int pointLightCount = pointLightView.size();
  m_pointLightUniformBlock.pointLightCount = std::min(pointLightCount, pointLightMaxCount);
  for (int i = 0; i < pointLightCount; i++) {
    if (i >= pointLightMaxCount) {
      LOG(WARNING) << FORMAT("the max count of light is {}", pointLightMaxCount);
      break;
    }
    const auto entity = pointLightView[i];
    const auto& lightComponent = pointLightView.get<const PointLightComponent>(entity);
    auto pos = lightComponent.m_light.GetPos();
    auto color = lightComponent.m_light.GetColor();
    auto farPlane = lightComponent.m_light.GetFarPlane();
    m_pointLightUniformBlock.lights[i].pos = pos;
    m_pointLightUniformBlock.lights[i].color = color;
    m_pointLightUniformBlock.lights[i].farPlane = farPlane;
  }
  m_pointLightUniformBuffer->SetData(&m_pointLightUniformBlock, pointLightBufSize, 0);
}

void
BlinnPhongRenderPass::Execute(const Scene* scene, const ResourceManager* resourceManager) {
  if (m_needToRecordComand) {
    RecordCommand(scene);
    m_needToRecordComand = false;
  }

  const auto editorCamera = scene->GetEditorCamrea();
  UpdateCameraUniformBuffer(editorCamera.get());
  SetUniformBuffer(const_cast<Scene*>(scene));
  m_commandBuffer->SubmitCommand();
}

}  // namespace Marbas
