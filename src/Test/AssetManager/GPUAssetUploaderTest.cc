#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "AssetManager/AssetManager.hpp"
#include "AssetManager/GPUAssetUpLoader.hpp"
#include "FakeClass/FakeRHIFactory.hpp"

namespace Marbas::Test {

struct CustomAsset final : public AssetBase {
  int i = 0;

  template <typename Archive>
  void
  serialize(Archive& ar) {
    ar(i);
  }

  static std::shared_ptr<CustomAsset>
  Load(const Path& path) {
    // path的内容如下: res://icon.png
    auto assert = std::make_shared<CustomAsset>();
    assert->i = 1;
    return assert;
  }
};

class CustomGPUAsset : public GPUAssetBase<CustomGPUAsset> {
  friend class GPUAssetBase<CustomGPUAsset>;

 public:
  CustomGPUAsset(RHIFactory* rhiFactory) : m_rhiFactory(rhiFactory) {}

  ~CustomGPUAsset() {
    if (buffer != nullptr) {
      auto bufferCtx = m_rhiFactory->GetBufferContext();
      bufferCtx->DestroyBuffer(buffer);
    }
  }

  static std::shared_ptr<CustomGPUAsset>
  LoadToGPU(const std::shared_ptr<CustomAsset>& asset, RHIFactory* rhiFactory) {
    auto gpuAsset = std::make_shared<CustomGPUAsset>(rhiFactory);
    auto bufferCtx = rhiFactory->GetBufferContext();
    gpuAsset->buffer = bufferCtx->CreateBuffer(BufferType::INDEX_BUFFER, nullptr, 0, false);
    return gpuAsset;
  }

 protected:
  void
  Update(const std::shared_ptr<CustomAsset>& asset) {
    this->i = asset->i;
  }

 public:
  int i = 0;

 private:
  RHIFactory* m_rhiFactory = nullptr;
  Buffer* buffer = nullptr;
};

class GPUAssetUpLoaderTest : public ::testing::Test {
  void
  SetUp() override {
    using ::testing::_;
    using ::testing::Return;

    m_rhiFactory = new FakeRHIFactory();
    m_bufferContext = static_cast<MockBufferContext*>(m_rhiFactory->GetBufferContext());
    m_pipelineContext = static_cast<MockPipelineContext*>(m_rhiFactory->GetPipelineContext());

    if (std::filesystem::exists(projectDir)) {
      std::filesystem::remove_all(projectDir);
    }
    m_assertRegistry->SetProjectDir(projectDir);
    m_assetManager->ClearCache();
    m_assetManager->ClearAll();
  }

  void
  TearDown() override {
    m_gpuAssetManager->Clear();
    delete m_rhiFactory;
  }

 protected:
  Path projectDir = "AssertTestDir";
  FakeRHIFactory* m_rhiFactory;
  AssetRegistryImpl* m_assertRegistry = AssetRegistry::GetInstance();
  GPUAssetManagerType<CustomGPUAsset>* m_gpuAssetManager = GPUAssetManager<CustomGPUAsset>::GetInstance();
  AssetManagerType<CustomAsset>* m_assetManager = AssetManager<CustomAsset>::GetInstance();
  MockBufferContext* m_bufferContext;
  MockPipelineContext* m_pipelineContext;
};

TEST_F(GPUAssetUpLoaderTest, CreateGPUAssert) {
  using ::testing::_;
  // create a temp assert
  auto assert = m_assetManager->Create("res://customRes.res");

  EXPECT_CALL(*m_bufferContext, CreateBuffer(_, _, _, _)).Times(1);

  m_gpuAssetManager->Create(assert, m_rhiFactory);
}

TEST_F(GPUAssetUpLoaderTest, GetGPUAssert) {
  using ::testing::_;
  // create a temp assert
  auto assert = m_assetManager->Create("res://customRes.res");
  EXPECT_CALL(*m_bufferContext, CreateBuffer(_, _, _, _)).Times(0);
  ASSERT_THROW(m_gpuAssetManager->Get(assert->GetUid()), AssetException);

  EXPECT_CALL(*m_bufferContext, CreateBuffer(_, _, _, _)).Times(1);
  m_gpuAssetManager->Create(assert, m_rhiFactory);
  auto gpuGetAssert = m_gpuAssetManager->Get(assert->GetUid());
  ASSERT_NE(gpuGetAssert, nullptr);
}

TEST_F(GPUAssetUpLoaderTest, ExistsAsset) {
  auto asset = m_assetManager->Create("res://customRes.res");
  ASSERT_FALSE(m_gpuAssetManager->Exists(asset->GetUid()));
  m_gpuAssetManager->Create(asset, m_rhiFactory);
  ASSERT_TRUE(m_gpuAssetManager->Exists(asset->GetUid()));
}

TEST_F(GPUAssetUpLoaderTest, UpdateAsset) {
  using ::testing::_;
  // create a temp assert
  auto assert = m_assetManager->Create("res://customRes.res");
  EXPECT_CALL(*m_bufferContext, CreateBuffer(_, _, _, _)).Times(0);
  ASSERT_THROW(m_gpuAssetManager->Get(assert->GetUid()), AssetException);

  EXPECT_CALL(*m_bufferContext, CreateBuffer(_, _, _, _)).Times(1);
  m_gpuAssetManager->Create(assert, m_rhiFactory);
  auto gpuAsset = m_gpuAssetManager->Get(assert->GetUid());
  ASSERT_NE(gpuAsset, nullptr);

  assert->i = 3;
  m_gpuAssetManager->Update(assert);
  ASSERT_EQ(gpuAsset->i, 3);
}

}  // namespace Marbas::Test
