#include <gtest/gtest.h>

#include "AssetManager/Singleton.hpp"
#include "Common/Common.hpp"

namespace Marbas::Test {

class SingletonTest : public ::testing::Test {};

TEST_F(SingletonTest, CreateSingleton) {
  struct MyClass {
    int a = 0;
  };

  auto* instance = Singleton<MyClass>::GetInstance();
  instance->a = 1;

  auto* newInstance = Singleton<MyClass>::GetInstance();

  ASSERT_EQ(newInstance->a, 1);
}

}  // namespace Marbas::Test
