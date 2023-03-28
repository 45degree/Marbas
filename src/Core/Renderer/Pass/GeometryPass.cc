#include "GeometryPass.hpp"

#include <nameof.hpp>

#include "AssetManager/GPUAssetUpLoader.hpp"
#include "AssetManager/ModelAsset.hpp"
#include "Core/Common.hpp"
#include "Core/Renderer/GBuffer.hpp"
#include "Core/Scene/Component/HierarchyComponent.hpp"
#include "Core/Scene/Component/RenderMeshComponent.hpp"

namespace Marbas {

GeometryPass::GeometryPass(const GeometryPassCreateInfo& createInfo)
    : m_normalTexture(createInfo.normalTexture),
      m_positionTexture(createInfo.positionTexture),
      m_metallicTexture(createInfo.metallicTexture),
      m_roughnessTexture(createInfo.roughnessTexture),
      m_depthTexture(createInfo.depthTexture),
      m_colorTexture(createInfo.colorTexture),
      m_aoTexture(createInfo.aoTexture),
      m_rhiFactory(createInfo.rhiFactory),
      m_scene(createInfo.scene),
      m_width(createInfo.width),
      m_height(createInfo.height) {
  auto pipelineContext = m_rhiFactory->GetPipelineContext();
  auto bufferContext = m_rhiFactory->GetBufferContext();

  SamplerCreateInfo samplerCreateInfo{
      .filter = Marbas::Filter::MIN_MAG_MIP_LINEAR,
      .addressU = Marbas::SamplerAddressMode::WRAP,
      .addressV = Marbas::SamplerAddressMode::WRAP,
      .addressW = Marbas::SamplerAddressMode::WRAP,
      .comparisonOp = Marbas::ComparisonOp::ALWAYS,
      .mipLodBias = 0,
      .minLod = 0,
      .maxLod = 0,
      .borderColor = Marbas::BorderColor::IntOpaqueBlack,
  };
  m_sampler = pipelineContext->CreateSampler(samplerCreateInfo);

  // create camera matrix buffer
  constexpr auto cameraBufferSize = sizeof(CameraMatrix);
  m_cameraBuffer = bufferContext->CreateBuffer(BufferType::UNIFORM_BUFFER, &m_cameraMatrix, cameraBufferSize, false);

  // create empty image
  m_emptyImage = bufferContext->CreateImage(ImageCreateInfo{});
  m_emptyImageView = bufferContext->CreateImageView(ImageViewCreateInfo{
      .image = m_emptyImage,
      .type = ImageViewType::TEXTURE2D,
      .baseLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  });
}

void
GeometryPass::SetUp(RenderGraphGraphicsBuilder& builder) {
  builder.WriteTexture(m_colorTexture);
  builder.WriteTexture(m_normalTexture);
  builder.WriteTexture(m_positionTexture);
  builder.WriteTexture(m_aoTexture);
  builder.WriteTexture(m_roughnessTexture);
  builder.WriteTexture(m_metallicTexture);
  builder.WriteTexture(m_depthTexture, TextureAttachmentType::DEPTH);

  RenderGraphPipelineCreateInfo pipelineCreateInfo;

  Vector<DescriptorSetLayoutBinding> bindings = {
      DescriptorSetLayoutBinding{0, DescriptorType::IMAGE},
      DescriptorSetLayoutBinding{1, DescriptorType::IMAGE},
      DescriptorSetLayoutBinding{2, DescriptorType::IMAGE},
      DescriptorSetLayoutBinding{3, DescriptorType::IMAGE},
      DescriptorSetLayoutBinding{4, DescriptorType::IMAGE},
      DescriptorSetLayoutBinding{0, DescriptorType::UNIFORM_BUFFER},
      DescriptorSetLayoutBinding{1, DescriptorType::UNIFORM_BUFFER},
      DescriptorSetLayoutBinding{2, DescriptorType::UNIFORM_BUFFER},
  };

  pipelineCreateInfo.AddShader(ShaderType::VERTEX_SHADER, "Shader/geometry.vert.spv");
  pipelineCreateInfo.AddShader(ShaderType::FRAGMENT_SHADER, "Shader/geometry.frag.spv");
  pipelineCreateInfo.SetPipelineLayout(bindings);
  pipelineCreateInfo.SetMultiSamples(SampleCount::BIT1);
  pipelineCreateInfo.SetColorAttachmentsDesc({
      {
          .initAction = AttachmentInitAction::CLEAR,
          .finalAction = AttachmentFinalAction::READ,
          .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
          .sampleCount = SampleCount::BIT1,
          .format = GBuffer_Color::format,
      },
      {
          .initAction = AttachmentInitAction::CLEAR,
          .finalAction = AttachmentFinalAction::READ,
          .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
          .sampleCount = SampleCount::BIT1,
          .format = GBuffer_Normals::format,
      },
      {
          .initAction = AttachmentInitAction::CLEAR,
          .finalAction = AttachmentFinalAction::READ,
          .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
          .sampleCount = SampleCount::BIT1,
          .format = GBuffer_Position::format,
      },
      {
          .initAction = AttachmentInitAction::CLEAR,
          .finalAction = AttachmentFinalAction::READ,
          .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
          .sampleCount = SampleCount::BIT1,
          .format = GBuffer_AmbientOcclusion::format,
      },
      {
          .initAction = AttachmentInitAction::CLEAR,
          .finalAction = AttachmentFinalAction::READ,
          .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
          .sampleCount = SampleCount::BIT1,
          .format = GBuffer_Roughness::format,
      },
      {
          .initAction = AttachmentInitAction::CLEAR,
          .finalAction = AttachmentFinalAction::READ,
          .usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET,
          .sampleCount = SampleCount::BIT1,
          .format = GBuffer_Metallic::format,
      },
  });
  pipelineCreateInfo.SetDepthAttachmentDesc({
      .initAction = AttachmentInitAction::CLEAR,
      .finalAction = AttachmentFinalAction::READ,
      .usage = ImageUsageFlags::DEPTH_STENCIL,
      .sampleCount = SampleCount::BIT1,
  });
  pipelineCreateInfo.SetVertexInputElementDesc(GetMeshVertexInfoLayout());
  pipelineCreateInfo.SetVertexInputElementView(GetMeshVertexViewInfo());
  pipelineCreateInfo.SetBlendAttachments({
      BlendAttachment{false},
      BlendAttachment{false},
      BlendAttachment{false},
      BlendAttachment{false},
      BlendAttachment{false},
      BlendAttachment{false},
  });
  pipelineCreateInfo.SetBlendConstance(0, 0, 0, 0);
  builder.SetPipelineInfo(pipelineCreateInfo);

  builder.SetFramebufferSize(m_width, m_height, 1);
}

void
GeometryPass::Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandList) {
  auto& world = m_scene->GetWorld();
  auto camera = m_scene->GetEditorCamera();
  auto view = world.view<ModelSceneNode, RenderComponent>();

  auto* bufferContext = m_rhiFactory->GetBufferContext();
  auto* pipelineContext = m_rhiFactory->GetPipelineContext();

  auto modelManager = AssetManager<ModelAsset>::GetInstance();
  auto modelGPUManager = GPUAssetManager<ModelGPUAsset>::GetInstance();

  /**
   * load all model and calculate the sum of mesh
   */

  int size = 0;
  for (auto&& [entity, modelSceneNode] : view.each()) {
    auto modelAsset = modelManager->Get(modelSceneNode.modelPath);
    size += modelAsset->GetMeshCount();
  }
  commandList.SetDescriptorSetCount(size);

  // update camera buffer
  m_cameraMatrix.up = camera->GetUpVector();
  m_cameraMatrix.pos = camera->GetPosition();
  m_cameraMatrix.right = camera->GetRightVector();
  m_cameraMatrix.projection = camera->GetProjectionMatrix();
  m_cameraMatrix.view = camera->GetViewMatrix();
  bufferContext->UpdateBuffer(m_cameraBuffer, &m_cameraMatrix, sizeof(CameraMatrix), 0);

  commandList.Begin({
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {1, 1},
  });

  for (auto&& [entity, modelSceneNode] : view.each()) {
    if (modelSceneNode.modelPath == "res://") {
      continue;
    }

    glm::mat4 model = glm::mat4(1.0);
    if (world.any_of<TransformComp>(entity)) {
      const auto& transformComp = world.get<TransformComp>(entity);
      model = transformComp.GetGlobalTransform();
    }

    auto modelAsset = modelManager->Get(modelSceneNode.modelPath);
    // Create GPU Asset
    // TODO: move this to scene update
    if (!modelGPUManager->Exists(modelAsset->GetUid())) {
      modelGPUManager->Create(modelAsset, m_rhiFactory);
    }
    modelGPUManager->Update(modelAsset);
    auto modelGPUAsset = modelGPUManager->Get(modelAsset->GetUid());
    for (auto& meshGPUAsset : modelGPUAsset->m_meshGPUAsset) {
      auto& indexCount = meshGPUAsset->m_indexCount;

      // update transform matrix
      bufferContext->UpdateBuffer(meshGPUAsset->m_transformBuffer, &model, sizeof(model), 0);

      RenderArgument argument;
      argument.BindUniformBuffer(0, m_cameraBuffer);
      argument.BindUniformBuffer(1, meshGPUAsset->m_materialInfoBuffer);
      argument.BindUniformBuffer(2, meshGPUAsset->m_transformBuffer);

      ImageView* imageView = nullptr;
      imageView = m_emptyImageView;
      if (meshGPUAsset->m_materialInfo.hasDiffuseTex) {
        imageView = meshGPUAsset->m_diffuseTexture->GetImageView(0, 1, 0, 1);
      }
      argument.BindImage(0, m_sampler, imageView);

      imageView = m_emptyImageView;
      if (meshGPUAsset->m_materialInfo.hasAoTex) {
        imageView = meshGPUAsset->m_aoTexture->GetImageView(0, 1, 0, 1);
      }
      argument.BindImage(1, m_sampler, imageView);

      imageView = m_emptyImageView;
      if (meshGPUAsset->m_materialInfo.hasNormalTex) {
        imageView = meshGPUAsset->m_normalTexture->GetImageView(0, 1, 0, 1);
      }
      argument.BindImage(2, m_sampler, imageView);

      imageView = m_emptyImageView;
      if (meshGPUAsset->m_materialInfo.hasRoughnessTex) {
        imageView = meshGPUAsset->m_roughnessTexture->GetImageView(0, 1, 0, 1);
      }
      argument.BindImage(3, m_sampler, imageView);

      imageView = m_emptyImageView;
      if (meshGPUAsset->m_materialInfo.hasMetallicTex) {
        imageView = meshGPUAsset->m_metallicTexture->GetImageView(0, 1, 0, 1);
      }
      argument.BindImage(4, m_sampler, imageView);

      commandList.BindVertexBuffer(meshGPUAsset->m_vertexBuffer);
      commandList.BindIndexBuffer(meshGPUAsset->m_indexBuffer);
      commandList.BindArgument(argument);
      commandList.DrawIndex(indexCount, 1, 0, 0, 0);
    }
  }

  commandList.End();
}

}  // namespace Marbas
