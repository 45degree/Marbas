#pragma once

#include <gtest/gtest.h>

#include "AssetManager/AssetRegistry.hpp"

namespace Marbas::Test {

class AssertRegistryTest : public ::testing::Test {
 public:
  void
  SetUp() override {
    if (std::filesystem::exists(projectDir)) {
      std::filesystem::remove_all(projectDir);
    }
  }

  void
  TearDown() override {}

 protected:
  Path projectDir = "AssertTestDir";
};

TEST_F(AssertRegistryTest, ResourceRegistryTest) {
  auto* instance = AssetRegistry::GetInstance();

  ASSERT_NO_THROW(instance->SetProjectDir(projectDir));
};

TEST_F(AssertRegistryTest, InsertResurcePath) {
  auto* instance = AssetRegistry::GetInstance();
  instance->SetProjectDir(projectDir);
  auto uid1 = instance->CreateOrFindAssertUid("res://icon.png");
  auto uid2 = instance->CreateOrFindAssertUid("res://icon.png");

  ASSERT_EQ(uid1, uid2);
}

TEST_F(AssertRegistryTest, SaveAssertRegistry) {
  auto* instance = AssetRegistry::GetInstance();
  instance->SetProjectDir(projectDir);
  auto uid1 = instance->CreateOrFindAssertUid("res://icon.png");
  auto unusedUid = instance->CreateOrFindAssertUid("res://icon2.png");
  instance->SaveAllAssert();

  instance->SetProjectDir(projectDir);
  auto uid2 = instance->CreateOrFindAssertUid("res://icon.png");
  ASSERT_EQ(uid1, uid2);
  ASSERT_EQ(2, instance->AssertCount());
}

TEST_F(AssertRegistryTest, DontInsertForErrorPath) {
  auto* instance = AssetRegistry::GetInstance();
  instance->SetProjectDir(projectDir);

  ASSERT_THROW(instance->CreateOrFindAssertUid("./icon.png"), std::out_of_range);
  ASSERT_EQ(0, instance->AssertCount());
}

}  // namespace Marbas::Test
