#pragma once

#include <cstdint>

#include "Common/Common.hpp"

namespace Marbas {

class RenderGraphGraphicsBuilder;
class RenderGraphComputeBuilder;
class RenderGraph;

namespace details {

enum class RenderGraphNodeType {
  Pass,
  Resource,
};

class RenderGraphNode {
 public:
  RenderGraphNode(StringView name, RenderGraphNodeType type) : name(name), type(type) {}
  virtual ~RenderGraphNode() = default;

  const StringView
  GetName() const {
    return name;
  }

  const Vector<RenderGraphNode*>&
  GetInputs() const {
    return inputs;
  }

  const Vector<RenderGraphNode*>&
  GetOutputs() const {
    return outputs;
  }

 protected:
  friend class Marbas::RenderGraphGraphicsBuilder;
  friend class Marbas::RenderGraphComputeBuilder;
  friend class Marbas::RenderGraph;

  String name;
  RenderGraphNodeType type;
  std::vector<RenderGraphNode*> inputs;
  std::vector<RenderGraphNode*> outputs;
};

}  // namespace details

}  // namespace Marbas
