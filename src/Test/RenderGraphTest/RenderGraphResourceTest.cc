#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Core/Renderer/RenderGraph/RenderGraphResource.hpp"
#include "Test/FakeClass/FakeRHIFactory.hpp"

namespace Marbas {

class RenderGraphResourceTest : public ::testing::Test {
 public:
  void
  SetUp() override {
    m_rhiFactory = new FakeRHIFactory();
    m_bufferContext = static_cast<MockBufferContext*>(m_rhiFactory->GetBufferContext());
    m_pipelineContext = static_cast<MockPipelineContext*>(m_rhiFactory->GetPipelineContext());
  }

  void
  TearDown() override {
    delete m_rhiFactory;
  }

 protected:
  FakeRHIFactory* m_rhiFactory;
  MockBufferContext* m_bufferContext;
  MockPipelineContext* m_pipelineContext;
};

TEST_F(RenderGraphResourceTest, CreateTextureResource) {
  using ::testing::_;
  EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(1);
  EXPECT_CALL(*m_bufferContext, CreateImageView(_)).Times(1);

  ImageCreateInfo createInfo;
  createInfo.imageDesc = Image2DDesc();
  createInfo.usage = ImageUsageFlags::SHADER_READ | ImageUsageFlags::COLOR_RENDER_TARGET;
  createInfo.width = 800;
  createInfo.height = 600;
  createInfo.format = ImageFormat::RGBA;
  createInfo.mipMapLevel = 1;
  createInfo.sampleCount = SampleCount::BIT1;

  details::RenderGraphTexture texture("test", m_rhiFactory, createInfo);
  texture.Create();
  auto* imageView = texture.GetImageView();
  auto* imageView2 = texture.GetImageView();
}

};  // namespace Marbas
