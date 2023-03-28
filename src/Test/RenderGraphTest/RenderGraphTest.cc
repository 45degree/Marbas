#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Core/Renderer/RenderGraph/RenderCommandList.hpp"
#include "Core/Renderer/RenderGraph/RenderGraph.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphBuilder.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphRegistry.hpp"
#include "Core/Renderer/RenderGraph/RenderGraphResource.hpp"
#include "Test/FakeClass/FakeRHIFactory.hpp"

namespace Marbas {

class RenderGraphTest : public ::testing::Test {
 public:
  void
  SetUp() override {
    using ::testing::_;
    using ::testing::Return;

    m_rhiFactory = new FakeRHIFactory();
    m_bufferContext = static_cast<MockBufferContext*>(m_rhiFactory->GetBufferContext());
    m_pipelineContext = static_cast<MockPipelineContext*>(m_rhiFactory->GetPipelineContext());
    m_renderGraphResourceManager = std::make_shared<RenderGraphResourceManager>(m_rhiFactory);

    ON_CALL(*m_bufferContext, CreateCommandBuffer(_)).WillByDefault(Return(&m_mockCommandBuffer));
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
  MockCommandBuffer m_mockCommandBuffer;
  std::shared_ptr<RenderGraphResourceManager> m_renderGraphResourceManager;
};

TEST_F(RenderGraphTest, CreatePass) {
  EXPECT_CALL(*m_rhiFactory, CreateGPUSemaphore()).Times(0);

  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);
  int init = 0;

  graph.AddPass("name", [&](RenderGraphGraphicsBuilder& builder) {
    return [&](RenderGraphRegistry& registry, GraphicsRenderCommandList& commandBuffer) { init = 1; };
  });
  graph.Compile();
  graph.Execute(nullptr, nullptr);

  ASSERT_EQ(init, 1);
}

TEST_F(RenderGraphTest, MultiPass) {
  EXPECT_CALL(*m_rhiFactory, CreateGPUSemaphore()).Times(1);

  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);

  int init = 0;
  graph.AddPass("pass1", [&](RenderGraphGraphicsBuilder& builder) {
    return [&](RenderGraphRegistry& registry, GraphicsRenderCommandList& commandBuffer) {
      ASSERT_EQ(init, 0);
      init = 1;
    };
  });

  graph.AddPass("pass2", [&](RenderGraphGraphicsBuilder& builder) {
    return [&](RenderGraphRegistry& registry, GraphicsRenderCommandList& commandBuffer) {
      ASSERT_EQ(init, 1);
      init = 2;
    };
  });

  graph.Compile();
  graph.Execute(nullptr, nullptr);
}

TEST_F(RenderGraphTest, CreateStructPass) {
  EXPECT_CALL(*m_rhiFactory, CreateGPUSemaphore()).Times(0);

  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);
  struct TestStruct {
    int& m_init;

    TestStruct(int& init) : m_init(init) {}

    void
    SetUp(RenderGraphGraphicsBuilder& builder) {}

    void
    Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandBuffer) {
      m_init = 1;
    };
  };

  int init = 0;
  graph.AddPass<TestStruct>("name", init);
  graph.Compile();
  graph.Execute(nullptr, nullptr);

  ASSERT_EQ(init, 1);
}

TEST_F(RenderGraphTest, CreateRenderGraph) {
  using ::testing::_;
  using ::testing::Return;

  // create a texture
  ImageCreateInfo imageCreateInfo;
  imageCreateInfo.imageDesc = Image2DDesc();
  imageCreateInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  imageCreateInfo.width = 800;
  imageCreateInfo.height = 600;
  imageCreateInfo.format = ImageFormat::RGBA;
  imageCreateInfo.mipMapLevel = 1;
  imageCreateInfo.sampleCount = SampleCount::BIT1;

  Image image;

  EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(1).WillOnce(Return(&image));
  RenderGraph renderGraph(m_rhiFactory, m_renderGraphResourceManager);
  auto outputTextureHandler = m_renderGraphResourceManager->CreateTexture("output", imageCreateInfo);
  renderGraph.AddPass("name", [&](RenderGraphGraphicsBuilder& builder) {
    builder.WriteTexture(outputTextureHandler);
    return [=, &image](RenderGraphRegistry& registry, GraphicsRenderCommandList& commandBuffer) {
      ASSERT_EQ(registry.GetRenderBackendTexture(outputTextureHandler), &image);
    };
  });

  renderGraph.Compile();
  renderGraph.Execute(nullptr, nullptr);
}

TEST_F(RenderGraphTest, AddRenderGraphResource) {
  using ::testing::_;
  using ::testing::Return;

  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);

  // create a texture
  ImageCreateInfo imageCreateInfo;
  imageCreateInfo.imageDesc = Image2DDesc();
  imageCreateInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  imageCreateInfo.width = 800;
  imageCreateInfo.height = 600;
  imageCreateInfo.format = ImageFormat::RGBA;
  imageCreateInfo.mipMapLevel = 1;
  imageCreateInfo.sampleCount = SampleCount::BIT1;

  Image image;

  // don't create any GPU resource
  EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(0);
  auto outputTextureHandler = m_renderGraphResourceManager->CreateTexture("output", imageCreateInfo);
  graph.AddPass("name", [&](RenderGraphGraphicsBuilder& builder) {
    builder.WriteTexture(outputTextureHandler);
    return [=, &image](RenderGraphRegistry& registry, GraphicsRenderCommandList& commandBuffer) {
      ASSERT_EQ(registry.GetRenderBackendTexture(outputTextureHandler), &image);
    };
  });

  // create GPU resource on compiling
  EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(1).WillOnce(Return(&image));

  graph.Compile();
  graph.Execute(nullptr, nullptr);
}

TEST_F(RenderGraphTest, CreateSubresource) {
  using ::testing::_;
  using ::testing::Return;

  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);

  // create a texture
  ImageCreateInfo imageCreateInfo;
  imageCreateInfo.imageDesc = Image2DDesc();
  imageCreateInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  imageCreateInfo.width = 800;
  imageCreateInfo.height = 600;
  imageCreateInfo.format = ImageFormat::RGBA;
  imageCreateInfo.mipMapLevel = 1;
  imageCreateInfo.sampleCount = SampleCount::BIT1;

  Image image;
  ImageView imageView;

  // don't create any GPU resource
  EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(0);
  EXPECT_CALL(*m_bufferContext, CreateImageView(_)).Times(0);
  auto outputTextureHandler = m_renderGraphResourceManager->CreateTexture("name", imageCreateInfo);
  graph.AddPass("name", [&](RenderGraphGraphicsBuilder& builder) {
    builder.WriteTexture(outputTextureHandler);
    return [=, &image, &imageView](RenderGraphRegistry& registry, GraphicsRenderCommandList& commandBuffer) {
      ASSERT_EQ(registry.GetRenderBackendTextureSubResource(outputTextureHandler, 0, 1, 0, 1), &imageView);
      ASSERT_EQ(registry.GetRenderBackendTexture(outputTextureHandler), &image);
    };
  });

  // create GPU resource on compiling
  EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(1).WillOnce(Return(&image));
  EXPECT_CALL(*m_bufferContext, CreateImageView(_)).Times(1).WillOnce(Return(&imageView));

  graph.Compile();
  graph.Execute(nullptr, nullptr);
}

TEST_F(RenderGraphTest, CreatePipelinePass) {
  using ::testing::_;
  using ::testing::Return;

  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);

  // create a texture
  ImageCreateInfo imageCreateInfo;
  imageCreateInfo.imageDesc = Image2DDesc();
  imageCreateInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  imageCreateInfo.width = 800;
  imageCreateInfo.height = 600;
  imageCreateInfo.format = ImageFormat::RGBA;
  imageCreateInfo.mipMapLevel = 1;
  imageCreateInfo.sampleCount = SampleCount::BIT1;

  auto outputTextureHandler = m_renderGraphResourceManager->CreateTexture("output", imageCreateInfo);
  auto inputTextureHandler = m_renderGraphResourceManager->CreateTexture("input", imageCreateInfo);

  SamplerCreateInfo samplerCreateInfo;
  auto sampler = m_rhiFactory->GetPipelineContext()->CreateSampler(samplerCreateInfo);
  auto bindBuffer = m_rhiFactory->GetBufferContext()->CreateBuffer(BufferType::UNIFORM_BUFFER, nullptr, 0, true);
  auto vertexBuffer = m_rhiFactory->GetBufferContext()->CreateBuffer(BufferType::VERTEX_BUFFER, nullptr, 0, true);
  auto indexBuffer = m_rhiFactory->GetBufferContext()->CreateBuffer(BufferType::INDEX_BUFFER, nullptr, 0, true);

  EXPECT_CALL(*m_pipelineContext, CreateFrameBuffer(_)).Times(1);
  // EXPECT_CALL(*m_pipelineContext, CreatePipeline(GraphicsPipeLineCreateInfo())).Times(1);
  graph.AddPass("name", [&](RenderGraphGraphicsBuilder& builder) {
    builder.WriteTexture(outputTextureHandler, TextureAttachmentType::COLOR);
    builder.ReadTexture(inputTextureHandler);

    Vector<DescriptorSetLayoutBinding> bindings = {
        {.bindingPoint = 0, .descriptorType = DescriptorType::UNIFORM_BUFFER},
        {.bindingPoint = 0, .descriptorType = DescriptorType::IMAGE},
    };

    RenderGraphPipelineCreateInfo createInfo;
    createInfo.SetPipelineLayout(bindings);
    builder.SetPipelineInfo(createInfo);

    builder.SetFramebufferSize(800, 600, 1);

    return [=](RenderGraphRegistry& registry, GraphicsRenderCommandList& cmdList) {
      cmdList.SetDescriptorSetCount(3);
      cmdList.Begin({});
      for (int i = 0; i < 3; i++) {
        RenderArgument argument;
        argument.BindUniformBuffer(0, bindBuffer);
        argument.BindImage(0, sampler, registry.GetRenderBackendTextureSubResource(inputTextureHandler, 0, 1, 0, 1));
        cmdList.BindArgument(argument);
        cmdList.BindVertexBuffer(vertexBuffer);
        cmdList.BindIndexBuffer(indexBuffer);
        cmdList.DrawIndex(0, 1, 0, 0, 0);
      }
      cmdList.End();
    };
  });

  graph.Compile();
}

TEST_F(RenderGraphTest, DisablePass) {
  EXPECT_CALL(*m_rhiFactory, CreateGPUSemaphore()).Times(0);

  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);
  struct TestStruct {
    int& m_init;

    TestStruct(int& init) : m_init(init) {}

    void
    SetUp(RenderGraphGraphicsBuilder& builder) {}

    void
    Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandBuffer) {
      m_init = 1;
    };

    bool
    IsEnable() {
      return false;
    }
  };

  int init = 0;
  graph.AddPass<TestStruct>("name", init);
  graph.Compile();
  graph.Execute(nullptr, nullptr);

  ASSERT_EQ(init, 0);
}

TEST_F(RenderGraphTest, ExecutePassAlone) {
  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);
  struct TestStruct {
    int& m_init;

    TestStruct(int& init) : m_init(init) {}

    void
    SetUp(RenderGraphGraphicsBuilder& builder) {}

    void
    Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandBuffer) {
      m_init = 1;
    };

    bool
    IsEnable() {
      return true;
    }
  };

  int init = 0;
  graph.AddPass<TestStruct>("name1", init);
  graph.AddPass<TestStruct>("name2", init);
  graph.Compile();

  graph.ExecuteAlone("name2", nullptr, nullptr, nullptr);
}

TEST_F(RenderGraphTest, MultiFrameBuffer) {
  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);
  struct TestStruct {
    void
    SetUp(RenderGraphGraphicsBuilder& builder) {}

    void
    Execute(RenderGraphRegistry& registry, GraphicsRenderCommandList& commandBuffer){
        // commandBuffer.Begin()
    };
  };
}

}  // namespace Marbas
