#include <gtest/gtest.h>

#include "Core/Renderer/GBuffer.hpp"
#include "RHIFactory.hpp"
#include "Test/FakeClass/FakeRHIFactory.hpp"

namespace Marbas {

class GBufferTest : public ::testing::Test {
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
  RHIFactory* m_rhiFactory;
  MockBufferContext* m_bufferContext;
  MockPipelineContext* m_pipelineContext;
};

TEST_F(GBufferTest, GetGBuffer) {
  GBufferRegistry gBufferMeteRegistry;

  EXPECT_CALL(*m_bufferContext, CreateImage(testing::_));
  EXPECT_CALL(*m_bufferContext, DestroyImage(testing::_)).Times(1);
  auto gbuffer = gBufferMeteRegistry.CreateGBuffer(m_rhiFactory, String(GBuffer_Color::typeName), 800, 600,
                                                   ImageUsageFlags::COLOR_RENDER_TARGET);
  EXPECT_NE(gbuffer, nullptr);

  gbuffer = nullptr;
}

TEST_F(GBufferTest, RegistryMetaInfo_NoTemplate) {
  GBufferRegistry gBufferRegistry;
  GBufferMetaInfo info{
      .format = ImageFormat::R32F,
      .imageViewType = ImageViewType::TEXTURE2D,
      .levels = 1,
      .layers = 1,
  };
  EXPECT_NO_THROW(gBufferRegistry.Emplace("MyColor", info));

  EXPECT_CALL(*m_bufferContext, CreateImage(testing::_)).Times(1);
  EXPECT_CALL(*m_bufferContext, DestroyImage(testing::_)).Times(1);
  EXPECT_CALL(*m_bufferContext, CreateImageView(testing::_)).Times(1);

  auto gbuffer = gBufferRegistry.CreateGBuffer(m_rhiFactory, "MyColor", 800, 600, ImageUsageFlags::COLOR_RENDER_TARGET);
  EXPECT_NE(gbuffer, nullptr);

  EXPECT_NO_THROW(gbuffer->GetWholeImageView());

  gbuffer = nullptr;
}

}  // namespace Marbas
