#include <gtest/gtest.h>

#include <Core/Tool/Quadtree.hpp>

namespace Marbas::Test {

class QuadtreeTest : public ::testing::Test {
 public:
};

TEST_F(QuadtreeTest, CreateTree) {
  Quadtree<glm::mat3> tree(glm::mat3(1.0));  // create a tree and set it's root value
  auto* rootNode = tree.GetRoot();
  ASSERT_EQ(rootNode->m_value, glm::mat3(1.0));
  ASSERT_EQ(rootNode->m_children[0], nullptr);
  ASSERT_EQ(rootNode->m_children[1], nullptr);
  ASSERT_EQ(rootNode->m_children[2], nullptr);
  ASSERT_EQ(rootNode->m_children[3], nullptr);
}

TEST_F(QuadtreeTest, CreateNode) {
  Quadtree<glm::mat3> tree(glm::mat3(1.0));
  auto node = tree.CreateNode(glm::mat3(1.0));
  ASSERT_EQ(node->m_value, glm::mat3(1.0));
  ASSERT_EQ(node->m_children[0], nullptr);
  ASSERT_EQ(node->m_children[1], nullptr);
  ASSERT_EQ(node->m_children[2], nullptr);
  ASSERT_EQ(node->m_children[3], nullptr);
}

TEST_F(QuadtreeTest, AddChildren) {
  Quadtree<glm::mat3> tree(1.0);
  auto* root = tree.GetRoot();
  auto node1 = tree.CreateNode(1.0);
  auto node1Ptr = node1.get();

  root->AddChild<1>(std::move(node1));
  ASSERT_EQ(root->m_children[1].get(), node1Ptr);
  ASSERT_EQ(node1Ptr->m_parent, root);
}

TEST_F(QuadtreeTest, SetUpdateStrategyTest) {
  Quadtree<glm::mat3> tree(glm::mat3(1.0));
  auto* rootNode = tree.GetRoot();
  auto node1 = tree.CreateNode(glm::mat3(1.0));
  auto* node1Ptr = node1.get();
  auto node2 = tree.CreateNode(1.0);
  auto* node2Ptr = node2.get();
  auto node3 = tree.CreateNode(1.0);
  auto* node3Ptr = node3.get();
  auto node4 = tree.CreateNode(1.0);
  auto* node4Ptr = node4.get();

  tree.SetUpdateStrategy<0>([](auto* node) {
    auto* parent = node->m_parent;
    constexpr static std::array<float, 9> _t = {0.5, 0, 0, 0, 0.5, 0, 0, 0, 1};
    const static glm::mat3 t = glm::make_mat3(_t.data());
    node->m_value = parent->m_value * t;
    return true;
  });
  tree.SetUpdateStrategy<1>([](QuadtreeNode<glm::mat3>* node) {
    constexpr static std::array<float, 9> _t = {0.5, 0, 0, 0, 0.5, 0, 0.5, 0, 1};
    const static glm::mat3 t = glm::make_mat3(_t.data());

    auto* parent = node->m_parent;
    node->m_value = parent->m_value * t;
    return true;
  });
  tree.SetUpdateStrategy<2>([](QuadtreeNode<glm::mat3>* node) {
    constexpr static std::array<float, 9> _t = {0.5, 0, 0, 0, 0.5, 0, 0, 0.5, 1};
    const static glm::mat3 t = glm::make_mat3(_t.data());

    auto* parent = node->m_parent;
    node->m_value = parent->m_value * t;
    return true;
  });
  tree.SetUpdateStrategy<3>([](QuadtreeNode<glm::mat3>* node) {
    constexpr static std::array<float, 9> _t = {0.5, 0, 0, 0, 0.5, 0, 0.5, 0.5, 1};
    const static glm::mat3 t = glm::make_mat3(_t.data());

    auto* parent = node->m_parent;
    node->m_value = parent->m_value * t;

    return true;
  });

  glm::vec3 originPoint(1, 1, 1.f);
  glm::vec3 resPoint;

  rootNode->AddChild<0>(std::move(node1));
  node1Ptr->AddChild<1>(std::move(node2));
  node2Ptr->AddChild<2>(std::move(node3));
  node2Ptr->AddChild<3>(std::move(node4));
  tree.Update();

  resPoint = node1Ptr->m_value * glm::vec3(1, 1.f, 1);
  ASSERT_EQ(resPoint, glm::vec3(0.5, 0.5, 1));

  resPoint = node2Ptr->m_value * glm::vec3(1, 1.f, 1);
  ASSERT_EQ(resPoint, glm::vec3(0.5, 0.25, 1));

  resPoint = node3Ptr->m_value * glm::vec3(1, 1.f, 1);
  ASSERT_EQ(resPoint, glm::vec3(0.375, 0.25, 1));

  resPoint = node4Ptr->m_value * glm::vec3(1, 1.f, 1);
  ASSERT_EQ(resPoint, glm::vec3(0.5, 0.25, 1));
}

}  // namespace Marbas::Test
