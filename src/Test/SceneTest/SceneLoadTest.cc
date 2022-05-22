#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "Common/Common.hpp"
#include "Core/Scene/Scene.hpp"
#include "Resource/ModelResource.hpp"

using Scene = Marbas::Scene;

class SceneLoadTest : public ::testing::Test {
 protected:
};

static Marbas::String tomlString = R"(
  [scene]
  name = "scene"
  resourceFile = "resource.toml"

  [[scene.node]]
  type = "Model"
  path = "Model.model"
  position = [2, 1, 0]
  rotate = [0, 0, 0]

  [[scene.node.mesh]]
  meshIndex = 0
  materialId = 1231313

  [[scene.node.mesh]]
  meshIndex = 1
  materialId = 1231313

  [[scene.node.node]]
  type = "Light"

  [[scene.node]]
  type = "Model"
  path = "Model.model"
  position = [ 0, 0, 0 ]
  rotate = [ 0, 0, 0 ]

  [[scene.node.mesh]]
  meshIndex = 0
  materialId = 1231313

  [[scene.node.mesh]]
  meshIndex = 1
  materialId = 1231313

  [[scene.node]]
  type = "Light"
)";

class MockResourceManager : public Marbas::ResourceManager {
  MOCK_CONST_METHOD0(GetModelResourceContainer,
                     std::shared_ptr<Marbas::IResourceContainer<Marbas::ModelResource>>());
};

TEST_F(SceneLoadTest, GetSceneName) {
  auto resourceManager = std::make_shared<Marbas::ResourceManager>();
  std::shared_ptr<Scene> scene = std::make_shared<Scene>(resourceManager);
  scene->ReadFromString(tomlString);
  auto name = scene->GetSceneName();
  ASSERT_EQ("scene", name);
}
