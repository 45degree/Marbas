#include <gtest/gtest.h>

#include "AssetManager/LRUCache.hpp"

namespace Marbas::Test {

class LRUCacheTest : public ::testing::Test {};

TEST_F(LRUCacheTest, AddElement) {
  LRUCache<int, std::string> lruCache(8);
  lruCache.insert(1, "123");
  ASSERT_EQ("123", lruCache.at(1));
}

TEST_F(LRUCacheTest, DeleteElementUnused) {
  LRUCache<int, std::string> lruCache(2);
  lruCache.insert(1, "1");
  lruCache.insert(2, "2");
  lruCache.insert(3, "3");

  ASSERT_FALSE(lruCache.existed(1));
  ASSERT_TRUE(lruCache.existed(2));
  ASSERT_TRUE(lruCache.existed(3));
}

TEST_F(LRUCacheTest, ThrowEpectionIfElementNotFound) {
  LRUCache<int, std::string> lruCache(2);
  lruCache.insert(1, "1");
  lruCache.insert(2, "2");

  ASSERT_THROW(lruCache.at(3), std::out_of_range);
}

TEST_F(LRUCacheTest, RemoveElement) {
  LRUCache<int, std::string> lruCache(2);

  lruCache.insert(1, "1");
  lruCache.remove(1);
  ASSERT_THROW(lruCache.at(1), std::out_of_range);
}

}  // namespace Marbas::Test
