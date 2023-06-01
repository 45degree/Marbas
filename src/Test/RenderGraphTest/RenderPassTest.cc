#include <gtest/gtest.h>

#include "Core/Renderer/RenderGraph/RenderGraph.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "FakeClass/FakeRHIFactory.hpp"

namespace Marbas::Test {

class RenderGraphPassTest : public ::testing::Test {
 public:
  RenderGraphPassTest() {
    m_fakeImageCreateInfo.imageDesc = Image2DDesc();
    m_fakeImageCreateInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
    m_fakeImageCreateInfo.width = 800;
    m_fakeImageCreateInfo.height = 600;
    m_fakeImageCreateInfo.format = ImageFormat::RGBA;
    m_fakeImageCreateInfo.mipMapLevel = 1;
    m_fakeImageCreateInfo.sampleCount = SampleCount::BIT1;
  }

  void
  SetUp() override {
    using ::testing::_;
    using ::testing::Return;

    m_rhiFactory = new FakeRHIFactory();
    m_bufferContext = static_cast<MockBufferContext*>(m_rhiFactory->GetBufferContext());
    m_pipelineContext = static_cast<MockPipelineContext*>(m_rhiFactory->GetPipelineContext());
    m_renderGraphResourceManager = std::make_shared<RenderGraphResourceManager>(m_rhiFactory);

    ON_CALL(*m_bufferContext, CreateGraphicsCommandBuffer()).WillByDefault(Return(&m_mockGraphicsCommandBuffer));
    ON_CALL(*m_bufferContext, CreateComputeCommandBuffer()).WillByDefault(Return(&m_mockComputeCommandBuffer));
  }

  void
  TearDown() override {
    m_renderGraphResourceManager = nullptr;
    delete m_rhiFactory;
  }

 protected:
  FakeRHIFactory* m_rhiFactory;
  MockBufferContext* m_bufferContext;
  MockPipelineContext* m_pipelineContext;
  MockGraphicsCommandBuffer m_mockGraphicsCommandBuffer;
  MockComputeCommandBuffer m_mockComputeCommandBuffer;
  std::shared_ptr<RenderGraphResourceManager> m_renderGraphResourceManager;

  ImageCreateInfo m_fakeImageCreateInfo;
  SamplerCreateInfo m_fakeSamplerCreateInfo;
  GraphicsPipeLineCreateInfo m_fakePipelineCreateInfo;
};

TEST_F(RenderGraphPassTest, CreateDescriptorSetFromRead) {
  using ::testing::_;
  using ::testing::Return;

  Image image;
  uintptr_t descriptorSet = 1;  // fake descriptor set
  uintptr_t fakeSampler = 1;    // fake sampler

  EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(2).WillRepeatedly(Return(&image));
  EXPECT_CALL(*m_pipelineContext, CreateDescriptorSet(_)).Times(2).WillRepeatedly(Return(descriptorSet));
  EXPECT_CALL(*m_pipelineContext, CreateSampler(_)).Times(2).WillRepeatedly(Return(fakeSampler));
  RenderGraph renderGraph(m_rhiFactory, m_renderGraphResourceManager);
  auto outputTextureHandler1 = m_renderGraphResourceManager->CreateTexture("output1", m_fakeImageCreateInfo);
  auto outputTextureHandler2 = m_renderGraphResourceManager->CreateTexture("output2", m_fakeImageCreateInfo);

  renderGraph.AddPass("pass1", [&](RenderGraphGraphicsBuilder& builder) {
    builder.WriteTexture(outputTextureHandler1);
    builder.BeginPipeline();
    builder.EndPipeline();
    return [](RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandBuffer) {};
  });
  renderGraph.AddPass("pass2", [&](RenderGraphGraphicsBuilder& builder) {
    auto pipelineCtx = m_rhiFactory->GetPipelineContext();
    auto sampler = pipelineCtx->CreateSampler(m_fakeSamplerCreateInfo);

    EXPECT_EQ(sampler, fakeSampler);

    builder.ReadTexture(outputTextureHandler1, sampler);
    builder.WriteTexture(outputTextureHandler2);
    builder.BeginPipeline();
    builder.EndPipeline();
    return [&](RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandBuffer) {
      auto* scene = registry.GetCurrentActiveScene();
      auto set = registry.GetInputDescriptorSet();
      ASSERT_EQ(set, descriptorSet);
    };
  });
  renderGraph.AddPass("pass3", [&](RenderGraphComputeBuilder& builder) {
    auto pipelineCtx = m_rhiFactory->GetPipelineContext();
    auto sampler = pipelineCtx->CreateSampler(m_fakeSamplerCreateInfo);
    builder.ReadTexture(outputTextureHandler2, sampler);
    return [&](RenderGraphComputeRegistry& registry, ComputeCommandBuffer& buffer) {
      auto set = registry.GetInputDescriptorSet();
      ASSERT_EQ(set, descriptorSet);
    };
  });

  renderGraph.Compile();
  renderGraph.Execute(nullptr, nullptr);
}

TEST_F(RenderGraphPassTest, AddRenderGraphResource) {
  using ::testing::_;
  using ::testing::Return;

  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);

  Image image;

  // don't create any GPU resource
  EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(0);
  auto outputTextureHandler = m_renderGraphResourceManager->CreateTexture("output", m_fakeImageCreateInfo);
  graph.AddPass("name", [&](RenderGraphGraphicsBuilder& builder) {
    builder.WriteTexture(outputTextureHandler);
    builder.BeginPipeline();
    builder.EndPipeline();
    return [=, &image](RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandBuffer) {};
  });

  // create GPU resource on compiling
  EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(1).WillOnce(Return(&image));

  graph.Compile();
  graph.Execute(nullptr, nullptr);
}

TEST_F(RenderGraphPassTest, AddComputePass) {
  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);
  struct TestComputePass {
    int& m_value;
    TestComputePass(int& value) : m_value(value) {}

    void
    SetUp(RenderGraphComputeBuilder& builder) {}

    void
    Execute(RenderGraphComputeRegistry& registry, ComputeCommandBuffer& commandBuffer) {
      m_value++;
    }
  };

  int i = 0;
  graph.AddPass<TestComputePass>("compute pass", i);

  graph.Compile();
  graph.Execute(nullptr);

  ASSERT_EQ(i, 1);
}

TEST_F(RenderGraphPassTest, AddLambdaComputePass) {
  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);

  int i = 0;
  graph.AddPass("compute pass", [&](RenderGraphComputeBuilder& builder) {
    return [&](RenderGraphComputeRegistry& registry, ComputeCommandBuffer& buffer) { i++; };
  });

  graph.Compile();
  graph.Execute(nullptr);

  ASSERT_EQ(i, 1);
}

TEST_F(RenderGraphPassTest, ComputeBuilderTest) {
  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);

  int i = 0;
  graph.AddPass("compute pass", [&](RenderGraphComputeBuilder& builder) {
    DescriptorSetArgument argument;
    builder.BeginPipeline();
    builder.AddShaderArgument(argument);
    builder.EndPipeline();
    return [&](RenderGraphComputeRegistry& registry, ComputeCommandBuffer& buffer) {
      // registry.
      i++;
    };
  });

  graph.Compile();
}

TEST_F(RenderGraphPassTest, CreatePipelinePass) {
  using ::testing::_;
  using ::testing::Return;

  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);

  auto outputTextureHandler = m_renderGraphResourceManager->CreateTexture("output", m_fakeImageCreateInfo);
  auto inputTextureHandler = m_renderGraphResourceManager->CreateTexture("input", m_fakeImageCreateInfo);

  auto sampler = m_rhiFactory->GetPipelineContext()->CreateSampler(m_fakeSamplerCreateInfo);
  auto bindBuffer = m_rhiFactory->GetBufferContext()->CreateBuffer(BufferType::UNIFORM_BUFFER, nullptr, 0, true);
  auto vertexBuffer = m_rhiFactory->GetBufferContext()->CreateBuffer(BufferType::VERTEX_BUFFER, nullptr, 0, true);
  auto indexBuffer = m_rhiFactory->GetBufferContext()->CreateBuffer(BufferType::INDEX_BUFFER, nullptr, 0, true);

  FrameBuffer fakeFrameBuffer;

  EXPECT_CALL(*m_pipelineContext, CreateFrameBuffer(_)).Times(1).WillOnce(Return(&fakeFrameBuffer));
  EXPECT_CALL(*m_pipelineContext, CreateDescriptorSet(_)).Times(2);
  graph.AddPass("name", [&](RenderGraphGraphicsBuilder& builder) {
    builder.WriteTexture(outputTextureHandler, TextureAttachmentType::COLOR);
    builder.ReadTexture(inputTextureHandler, sampler);

    DescriptorSetArgument argument1, argument2;
    argument1.Bind(0, DescriptorType::UNIFORM_BUFFER);
    argument1.Bind(0, DescriptorType::IMAGE);

    builder.BeginPipeline();
    builder.AddShaderArgument(argument1);
    builder.AddShaderArgument(argument2);
    builder.EndPipeline();

    builder.SetFramebufferSize(800, 600, 1);

    auto desciptorSet = m_rhiFactory->GetPipelineContext()->CreateDescriptorSet(argument1);
    m_rhiFactory->GetPipelineContext()->BindBuffer(BindBufferInfo{
        .descriptorSet = desciptorSet,
        .descriptorType = DescriptorType::UNIFORM_BUFFER,
        .bindingPoint = 0,
        .buffer = bindBuffer,
        .offset = 0,
        .arrayElement = 0,
    });

    return [=](RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& cmdList) {
      auto inputSet = registry.GetInputDescriptorSet();
      auto pipeline = registry.GetPipeline(0);
      auto framebuffer = registry.GetFrameBuffer();

      ASSERT_EQ(framebuffer, &fakeFrameBuffer);

      cmdList.Begin();
      cmdList.BeginPipeline(pipeline, framebuffer, {{0, 0, 0, 1}});
      for (int i = 0; i < 3; i++) {
        cmdList.BindDescriptorSet(0, {desciptorSet, inputSet});
        cmdList.BindVertexBuffer(vertexBuffer);
        cmdList.BindIndexBuffer(indexBuffer);
        cmdList.DrawIndexed(0, 1, 0, 0, 0);
      }
      cmdList.EndPipeline(pipeline);
      cmdList.End();
    };
  });

  graph.Compile();
}

}  // namespace Marbas::Test
