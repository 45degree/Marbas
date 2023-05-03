#include <gtest/gtest.h>

#include <AssetManager/AssetManager.hpp>
#include <AssetManager/AssetRegistry.hpp>
#include <AssetManager/ModelAsset.hpp>

namespace Marbas::Test {

struct CustomAsset : public AssetBase {
  int i = 0;

  template <typename Archive>
  void
  serialize(Archive& ar) {
    ar(i);
  }

  static std::shared_ptr<CustomAsset>
  Load(const AssetPath& path) {
    // path的内容如下: res://icon.png
    auto assert = std::make_shared<CustomAsset>();
    assert->i = 1;
    return assert;
  }
};

class AssetManagerTest : public ::testing::Test {
 public:
  void
  SetUp() override {
    if (std::filesystem::exists(projectDir)) {
      std::filesystem::remove_all(projectDir);
    }
    assetRegistry->SetProjectDir(projectDir);
    assetManager->ClearCache();
    assetManager->ClearAll();
  }

  void
  TearDown() override {}

 protected:
  Path projectDir = "AssertTestDir";
  AssetRegistryImpl* assetRegistry = AssetRegistry::GetInstance();
  AssetManagerType<CustomAsset>* assetManager = AssetManager<CustomAsset>::GetInstance();
};

TEST_F(AssetManagerTest, CreateAsset) {
  // create the assert and add the assert to the lru cache
  // if the assert is not load, then laod the assert
  // if the assert has beed loaded, return the assert
  auto assert = assetManager->Create("res://customRes.res");
  ASSERT_EQ(assert->i, 1);

  // check file
  auto assertPath = assetRegistry->GetAssertAbsolutePath(assert->GetUid());
  ASSERT_EQ(assertPath, projectDir / ".import/" / (std::to_string(assert->GetUid()) + ".data"));
  ASSERT_TRUE(std::filesystem::exists(assertPath));

  // throw exception if the assert is existed
  ASSERT_THROW(assetManager->Create("res://customRes.res"), AssetException);
}

TEST_F(AssetManagerTest, GetAsset) {
  // create a temp assert
  auto assert1 = assetManager->Create("res://customRes.res");

  auto assert2 = assetManager->Get("res://customRes.res");
  ASSERT_EQ(assert1, assert2);
}

TEST_F(AssetManagerTest, DeleteAsset) {
  // create a temp assert
  auto assert = assetManager->Create("res://customRes.res");
  assetManager->Delete("res://customRes.res");

  auto assertPath = projectDir / ".import/" / (std::to_string(assert->GetUid()) + ".data");
  ASSERT_FALSE(std::filesystem::exists(assertPath));
}

TEST_F(AssetManagerTest, ChangeCacheSize) {
  // create a temp assert
  assetManager->ResizeCache(2);
  Uid uid;
  {
    auto assert1 = assetManager->Create("res://customRes.res");
    auto assert2 = assetManager->Create("res://customRes2.res");
    uid = assert1->GetUid();
  }
  auto assert3 = assetManager->Create("res://customRes3.res");
  ASSERT_FALSE(assetManager->IsInCache(uid));
  ASSERT_FALSE(assetManager->IsInUse(uid));
}

TEST_F(AssetManagerTest, Tick) {
  Uid uid;
  assetManager->ResizeCache(2);
  {
    auto asset1 = assetManager->Create("res://customRes.res");
    auto asset2 = assetManager->Create("res://customRes2.res");
    uid = asset1->GetUid();
  }
  auto assert3 = assetManager->Create("res://customRes3.res");

  assetManager->Tick();
  ASSERT_FALSE(assetManager->IsInCache(uid));
  ASSERT_FALSE(assetManager->IsInUse(uid));
}

TEST_F(AssetManagerTest, RemoveAssetFromCache) {
  Uid uid;
  {
    auto asset = assetManager->Create("res://customRes.res");
    uid = asset->GetUid();
  }
  ASSERT_TRUE(assetManager->IsInCache(uid));
  assetManager->RemoveFromCache(uid);
  ASSERT_FALSE(assetManager->IsInCache(uid));
}

}  // namespace Marbas::Test
