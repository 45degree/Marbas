#include <gmock/gmock.h>
#include <gtest/gtest.h>

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
};

TEST_F(RenderGraphTest, CreatePass) {
  EXPECT_CALL(*m_rhiFactory, CreateGPUSemaphore()).Times(0);

  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);
  int init = 0;

  graph.AddPass("name", [&](RenderGraphGraphicsBuilder& builder) {
    builder.BeginPipeline();
    builder.EndPipeline();
    return [&](RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandBuffer) { init = 1; };
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
    builder.BeginPipeline();
    builder.EndPipeline();
    return [&](RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandBuffer) {
      ASSERT_EQ(init, 0);
      init = 1;
    };
  });

  graph.AddPass("pass2", [&](RenderGraphGraphicsBuilder& builder) {
    builder.BeginPipeline();
    builder.EndPipeline();
    return [&](RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandBuffer) {
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
    SetUp(RenderGraphGraphicsBuilder& builder) {
      builder.BeginPipeline();
      builder.EndPipeline();
    }

    void
    Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandBuffer) {
      m_init = 1;
    };
  };

  int init = 0;
  graph.AddPass<TestStruct>("name", init);
  graph.Compile();
  graph.Execute(nullptr, nullptr);

  ASSERT_EQ(init, 1);
}

TEST_F(RenderGraphTest, DisablePass) {
  EXPECT_CALL(*m_rhiFactory, CreateGPUSemaphore()).Times(0);

  RenderGraph graph(m_rhiFactory, m_renderGraphResourceManager);
  struct TestStruct {
    int& m_init;

    TestStruct(int& init) : m_init(init) {}

    void
    SetUp(RenderGraphGraphicsBuilder& builder) {
      builder.BeginPipeline();
      builder.EndPipeline();
    }

    void
    Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandBuffer) {
      m_init = 1;
    };

    bool
    IsEnable(RenderGraphGraphicsRegistry& registry) {
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
    SetUp(RenderGraphGraphicsBuilder& builder) {
      builder.BeginPipeline();
      builder.EndPipeline();
    }

    void
    Execute(RenderGraphGraphicsRegistry& registry, GraphicsCommandBuffer& commandBuffer) {
      m_init = 1;
    };

    bool
    IsEnable(RenderGraphGraphicsRegistry& registry) {
      return true;
    }
  };

  int init = 0;
  graph.AddPass<TestStruct>("name1", init);
  graph.AddPass<TestStruct>("name2", init);
  graph.Compile();

  graph.ExecuteAlone("name2", nullptr, nullptr, nullptr, nullptr);
}

}  // namespace Marbas
