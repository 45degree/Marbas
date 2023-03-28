// #include <gtest/gtest.h>
//
// #include "Core/Renderer/GBuffer.hpp"
// #include "Core/Renderer/RenderPassBase.hpp"
// #include "Test/FakeClass/FakeRHIFactory.hpp"
//
// namespace Marbas {
//
// class RenderPassTest : public ::testing::Test {
//  public:
//   RenderPassTest() {}
//
//   void
//   SetUp() override {
//     m_rhiFactory = new FakeRHIFactory();
//     m_bufferContext = static_cast<MockBufferContext*>(m_rhiFactory->GetBufferContext());
//     m_pipelineContext = static_cast<MockPipelineContext*>(m_rhiFactory->GetPipelineContext());
//   }
//
//   void
//   TearDown() override {
//     delete m_rhiFactory;
//   }
//
//  protected:
//   RHIFactory* m_rhiFactory;
//   MockBufferContext* m_bufferContext;
//   MockPipelineContext* m_pipelineContext;
//
//   GBufferRegistry m_gBufferMeteRegistry;
// };
//
// class FakeClass final : public unused::RenderPassBase {
//  public:
//   FakeClass(const String& passName, int width, int height, GBufferRegistry* registry, RHIFactory* rhiFactory)
//       : RenderPassBase(passName, registry, rhiFactory) {
//     AddInput("buffer1", String(GBuffer_Color::typeName));
//     AddOutput("buffer1", String(GBuffer_Color::typeName), ImageUsageFlags::COLOR_RENDER_TARGET);
//     AddOutput("buffer2", String(GBuffer_Color::typeName), ImageUsageFlags::COLOR_RENDER_TARGET);
//   };
//
//   Pipeline*
//   CreatePipeline(PipelineContext* pipelineContext) override {
//     return nullptr;
//   }
//
//   void
//   SetFrameBuffer(FrameBufferCreateInfo& createInfo) override {}
//
//   void
//   RecordCommandBuffer(Scene* scene) override {}
//
//   void
//   SubmitCommand(std::span<Semaphore*> waitSemaphore, std::span<Semaphore*> signalSemaphore) override {}
// };
//
// class FakeForwarPass final : public unused::ForwardRenderPassBase {
//  public:
//   FakeForwarPass(const String& passName, GBufferRegistry* registry, RHIFactory* rhiFactory)
//       : ForwardRenderPassBase(passName, registry, rhiFactory) {}
//
//   virtual ~FakeForwarPass() = default;
//
//  public:
//   void
//   RecordCommandBuffer(Scene* scene) override {}
//
//   void
//   SubmitCommand(std::span<Semaphore*> waitSemaphore, std::span<Semaphore*> signalSemaphore) override {}
//
//   Pipeline*
//   CreatePipeline(PipelineContext* pipelineContext) override {
//     return nullptr;
//   }
//
//   void
//   SetFrameBuffer(FrameBufferCreateInfo& createInfo) override {}
// };
//
// TEST_F(RenderPassTest, CreatePass) {
//   using ::testing::_;
//   using ::testing::Return;
//
//   Image fakeImage;
//   ImageView fakeImageView;
//
//   // create 2 images and image views for the render target
//   EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(2).WillRepeatedly(Return(&fakeImage));
//   EXPECT_CALL(*m_bufferContext, CreateImageView(_)).Times(2).WillRepeatedly(Return(&fakeImageView));
//   EXPECT_CALL(*m_bufferContext, DestroyImageView(_)).Times(2);
//   EXPECT_CALL(*m_bufferContext, DestroyImage(_)).Times(2);
//   EXPECT_CALL(*m_pipelineContext, CreateFrameBuffer(_)).Times(1);
//
//   auto pass = std::make_unique<FakeClass>("pass", 800, 600, &m_gBufferMeteRegistry, m_rhiFactory);
//   pass->Initialize(800, 600);
//   auto* buffer1View = pass->GetOutput("buffer1")->GetWholeImageView();
//   auto* buffer2View = pass->GetOutput("buffer2")->GetWholeImageView();
//   ASSERT_EQ(buffer1View, &fakeImageView);
//   ASSERT_EQ(buffer1View, &fakeImageView);
//
//   pass = nullptr;
// }
//
// TEST_F(RenderPassTest, ResizePass) {
//   using ::testing::_;
//   using ::testing::Return;
//
//   Image originImage, resizedImage;
//   ImageView originImageView, resizedImageView;
//
//   EXPECT_CALL(*m_bufferContext, CreateImage(_))
//       .Times(4)
//       .WillOnce(Return(&originImage))
//       .WillOnce(Return(&originImage))
//       .WillOnce(Return(&resizedImage))
//       .WillOnce(Return(&resizedImage));
//
//   EXPECT_CALL(*m_bufferContext, CreateImageView(_))
//       .Times(4)
//       .WillOnce(Return(&originImageView))
//       .WillOnce(Return(&originImageView))
//       .WillOnce(Return(&resizedImageView))
//       .WillOnce(Return(&resizedImageView));
//   EXPECT_CALL(*m_bufferContext, DestroyImageView(_)).Times(4);
//   EXPECT_CALL(*m_bufferContext, DestroyImage(_)).Times(4);
//   EXPECT_CALL(*m_pipelineContext, CreateFrameBuffer(_)).Times(2);
//
//   auto pass = std::make_unique<FakeClass>("pass", 800, 600, &m_gBufferMeteRegistry, m_rhiFactory);
//   pass->Initialize(800, 600);
//   pass->Resize(1920, 1080);
//   auto* buffer1View = pass->GetOutput("buffer1")->GetWholeImageView();
//
//   EXPECT_EQ(buffer1View, &resizedImageView);
// }
//
// TEST_F(RenderPassTest, LinkPass) {
//   using ::testing::_;
//   using ::testing::Return;
//
//   EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(4);
//   EXPECT_CALL(*m_bufferContext, CreateImageView(_)).Times(4);
//   EXPECT_CALL(*m_bufferContext, DestroyImageView(_)).Times(4);
//   EXPECT_CALL(*m_bufferContext, DestroyImage(_)).Times(4);
//   EXPECT_CALL(*m_pipelineContext, CreateFrameBuffer(_)).Times(2);
//
//   auto pass1 = std::make_unique<FakeClass>("pass1", 800, 600, &m_gBufferMeteRegistry, m_rhiFactory);
//   pass1->Initialize(800, 600);
//   auto pass2 = std::make_unique<FakeClass>("pass2", 800, 600, &m_gBufferMeteRegistry, m_rhiFactory);
//   pass2->Initialize(800, 600);
//
//   pass2->AddDependence("buffer1", pass1.get(), "buffer1");
//
//   auto* pass2Input = pass2->GetInput("buffer1")->GetWholeImageView();
//   auto* pass1Output = pass1->GetOutput("buffer1")->GetWholeImageView();
//   ASSERT_EQ(pass1Output, pass2Input);
// }
//
// TEST_F(RenderPassTest, LinkPass_SelfLink) {
//   using ::testing::_;
//   using ::testing::Return;
//
//   EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(2);
//   EXPECT_CALL(*m_bufferContext, CreateImageView(_)).Times(2);
//   EXPECT_CALL(*m_bufferContext, DestroyImageView(_)).Times(2);
//   EXPECT_CALL(*m_bufferContext, DestroyImage(_)).Times(2);
//   EXPECT_CALL(*m_pipelineContext, CreateFrameBuffer(_)).Times(1);
//
//   auto pass = std::make_unique<FakeClass>("pass", 800, 600, &m_gBufferMeteRegistry, m_rhiFactory);
//   pass->Initialize(800, 600);
//
//   ASSERT_THROW(pass->AddDependence("buffer1", pass.get(), "buffer1"), std::runtime_error);
//   ASSERT_THROW(pass->AddDependence("buffer1", pass.get(), "buffer2"), std::runtime_error);
// }
//
// TEST_F(RenderPassTest, AddNextForwarPass) {
//   using ::testing::_;
//   using ::testing::Return;
//
//   EXPECT_CALL(*m_bufferContext, CreateImage(_)).Times(2);
//   EXPECT_CALL(*m_bufferContext, CreateImageView(_)).Times(2);
//   EXPECT_CALL(*m_bufferContext, DestroyImageView(_)).Times(2);
//   EXPECT_CALL(*m_bufferContext, DestroyImage(_)).Times(2);
//   EXPECT_CALL(*m_pipelineContext, CreateFrameBuffer(_)).Times(2);
//
//   auto pass = std::make_unique<FakeClass>("pass", 800, 600, &m_gBufferMeteRegistry, m_rhiFactory);
//   auto forwardPass = std::make_unique<FakeForwarPass>("forwardPass", &m_gBufferMeteRegistry, m_rhiFactory);
//   pass->AddNextForwardPass(std::move(forwardPass));
//   pass->AddForwardLink("buffer1", "$forwardPass.buffer1");
//   pass->AddForwardLink("buffer2", "$forwardPass.buffer2");
//   pass->Initialize(800, 600);
// }
//
// }  // namespace Marbas
