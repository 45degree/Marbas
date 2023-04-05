#pragma once

#include <array>
#include <functional>
#include <memory>
#include <tuple>
#include <vector>

#include "Common/MathCommon.hpp"

namespace Marbas {

template <typename T>
class Quadtree;

template <typename T>
struct QuadtreeNode final {
  friend class Quadtree<T>;

  QuadtreeNode<T>* m_parent = nullptr;
  std::array<std::unique_ptr<QuadtreeNode<T>>, 4> m_children;
  T m_value;

 public:
  QuadtreeNode(const QuadtreeNode& obj) = delete;
  QuadtreeNode&
  operator=(const QuadtreeNode& obj) = delete;

 public:
  template <int N>
  void
  AddChild(std::unique_ptr<QuadtreeNode<T>> node) {
    static_assert(N >= 0 && N <= 3);
    node->m_parent = this;
    m_children[N] = std::move(node);
  }

 private:
  template <typename... Args>
  QuadtreeNode(Quadtree<T>* tree, Args&&... args)
      : m_value(std::forward<Args>(args)...), m_tree(tree), m_parent(nullptr), m_children({nullptr}) {}

 private:
  Quadtree<T>* m_tree;
};

template <typename T>
struct Quadtree {
 public:
  template <typename... Args>
  Quadtree(Args&&... args) {
    m_root = std::unique_ptr<QuadtreeNode<T>>(new QuadtreeNode(this, std::forward<Args>(args)...));
  }

 public:
  QuadtreeNode<T>*
  GetRoot() {
    return m_root.get();
  }

  template <typename... Args>
  std::unique_ptr<QuadtreeNode<T>>
  CreateNode(Args&&... args) {
    return std::unique_ptr<QuadtreeNode<T>>(new QuadtreeNode(this, std::forward<Args>(args)...));
  }

  template <int N>
  void
  SetUpdateStrategy(std::function<bool(QuadtreeNode<T>* node)> strategy) {
    static_assert(N >= 0 && N < 4);
    m_updateStrategies[N] = strategy;
  }

  void
  SetUpdateStrategy(std::function<bool(QuadtreeNode<T>* node)> strategy) {
    m_updateStrategies[0] = strategy;
    m_updateStrategies[1] = strategy;
    m_updateStrategies[2] = strategy;
    m_updateStrategies[3] = strategy;
  }

  void
  Update() {
    UpdateNode(m_root.get());
  }

 private:
  void
  UpdateNode(QuadtreeNode<T>* node) {
    for (int i = 0; i < node->m_children.size(); i++) {
      auto* child = node->m_children[i].get();
      if (child == nullptr) continue;

      if (m_updateStrategies[i](child)) {
        UpdateNode(child);
      }
    }
  }

 private:
  std::array<std::function<bool(QuadtreeNode<T>*)>, 4> m_updateStrategies;

  std::unique_ptr<QuadtreeNode<T>> m_root;
};

}  // namespace Marbas
