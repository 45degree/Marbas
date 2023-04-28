#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "AssetManager/AssetRegistry.hpp"
#include "Common/Common.hpp"
#include "Core/Scene/Scene.hpp"
#include "Core/Scene/System/SceneSystem.hpp"

namespace Marbas::Test {

class SceneLoadTest : public ::testing::Test {
 public:
  void
  SetUp() override {
    if (std::filesystem::exists(projectDir)) {
      std::filesystem::remove_all(projectDir);
    }
    m_assetRegistry->SetProjectDir(projectDir);
  }

 protected:
  AssetRegistryType* m_assetRegistry = AssetRegistry::GetInstance();
  Path projectDir = "SceneTestDir";
};

TEST_F(SceneLoadTest, SceneNodeTest) {
  auto scene = std::make_shared<Scene>();
  auto& world = scene->GetWorld();
  auto rootEntity = scene->GetRootNode();

  ASSERT_TRUE(world.any_of<TransformComp>(rootEntity));
}

TEST_F(SceneLoadTest, SaveLoadScene) {
  auto scenePath = m_assetRegistry->GetProjectDir() / "scene.scene";

  auto scene = std::make_shared<Scene>();
  auto& world = scene->GetWorld();

  auto rootNode = scene->GetRootNode();
  auto child = scene->AddChild(rootNode);

  auto& modelNode = world.emplace<ModelSceneNode>(child);
  modelNode.modelPath = "res://icon.png";

  scene->SaveToFile(scenePath);

  auto newScene = Scene::LoadFromFile(scenePath);
  auto newRootNode = newScene->GetRootNode();
  auto& newWorld = newScene->GetWorld();
  ASSERT_EQ(1, newScene->GetChildrenCount(newRootNode));
  ASSERT_TRUE(newWorld.any_of<ModelSceneNode>(newScene->GetChild(newRootNode, 0)));

  auto& newModelName = newWorld.get<ModelSceneNode>(newScene->GetChild(newRootNode, 0));
  ASSERT_EQ(modelNode.modelPath, newModelName.modelPath);
}

}  // namespace Marbas::Test
