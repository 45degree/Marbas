#pragma once

#include <gtest/gtest.h>

#include "AssetManager/AssetManager.hpp"
#include "Core/Scene/GPUDataPipeline/GPUDataPipeline.hpp"
#include "Core/Scene/Scene.hpp"
#include "FakeClass/FakeRHIFactory.hpp"

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

class GPUDataPipelineTest : public testing::Test {
 public:
  void
  SetUp() {
    m_scene = Scene();

    if (std::filesystem::exists(projectDir)) {
      std::filesystem::remove_all(projectDir);
    }
    assetRegistry->SetProjectDir(projectDir);
    assetManager->ClearCache();
    assetManager->ClearAll();
  }

 protected:
  Scene m_scene;
  FakeRHIFactory m_rhiFactory;

  Path projectDir = "GPUDataPipelineTestDir";
  AssetRegistryImpl* assetRegistry = AssetRegistry::GetInstance();
  AssetManagerType<CustomAsset>* assetManager = AssetManager<CustomAsset>::GetInstance();
};

TEST_F(GPUDataPipelineTest, DataFromEntity) {
  struct CustomComponent {
    int num = 1;
  };

  struct CustomData final : public GPUDataPipelineDataBase {
   public:
    Task<>
    Load(const CustomComponent& component) {
      m_num = component.num + 1;
      co_return;
    }

    Task<>
    Update(const CustomComponent& component) {
      m_num++;
      co_return;
    }

   public:
    int m_num = 0;
  };

  auto entity = m_scene.GetRootNode();
  auto& world = m_scene.GetWorld();
  auto& component = world.emplace<CustomComponent>(entity);

  auto& instance = *GPUDataPipelineFromEntityManager<CustomData>::GetInstance();
  instance.SetRHI(&m_rhiFactory);

  instance.Create(entity, component);
  auto data = instance.TryGet(entity);
  ASSERT_EQ(data->m_num, 2);

  component.num = 2;
  instance.Update(entity, component);
  ASSERT_EQ(data->m_num, 3);
}

TEST_F(GPUDataPipelineTest, DataFromAsset) {
  struct CustomData final : public GPUDataPipelineDataBase {
   public:
    Task<>
    Load(const CustomAsset& asset) {
      m_num = asset.i + 1;
      co_return;
    }

    Task<>
    Update(const CustomAsset& asset) {
      m_num++;
      co_return;
    }

   public:
    int m_num = 0;
  };

  auto* assetManager = AssetManager<CustomAsset>::GetInstance();
  auto asset = assetManager->Create("res://customRes.res");

  auto& instance = *GPUDataPipelineFromAssetManager<CustomData>::GetInstance();
  instance.SetRHI(&m_rhiFactory);

  instance.Create(*asset);
  auto data = instance.TryGet(*asset);
  ASSERT_EQ(data->m_num, 2);
}

}  // namespace Marbas::Test
