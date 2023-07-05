#pragma once

#include <functional>
#include <type_traits>

#include "Core/Scene/Scene.hpp"
#include "RenderGraphBuilder.hpp"
#include "RenderGraphNode.hpp"
#include "RenderGraphRegistry.hpp"

namespace Marbas {

enum class PassType {
  Graphics,
};

namespace details {

class RenderGraphPass : public RenderGraphNode {
 public:
  RenderGraphPass(StringView name, RHIFactory* rhiFactory);
  virtual ~RenderGraphPass();

  virtual void
  Initialize(RenderGraph* graph) = 0;

  virtual void
  Execute(RenderGraph* graph, void* userData) = 0;

  virtual void
  Submit(Semaphore* waitSemaphore, Semaphore* signedSemaphore, Fence* fence) = 0;

  virtual bool
  IsEnable(RenderGraph* graph, void* userData) = 0;

 protected:
  RHIFactory* m_rhiFactory;
};

struct InputDesc {
  virtual ~InputDesc() = default;

  virtual void
  Bind(RenderGraph* graph, PipelineContext* ctx, uintptr_t set, uint16_t bindingPoint) const = 0;

  virtual void
  SetArgument(DescriptorSetArgument& argument, uint16_t bindingPoint) const = 0;
};

struct ImageDesc {
  virtual ~ImageDesc() = default;

  RenderGraphTextureHandler m_handler;
  uint32_t m_baseLayer;
  uint32_t m_layerCount;
  uint32_t m_baseLevel;
  uint32_t m_levelCount;

  ImageView*
  GetImageView(RenderGraph* graph) const;
};

struct CombineImageDesc final : public InputDesc, ImageDesc {
  uintptr_t m_sampler;

 public:
  ~CombineImageDesc() override = default;

  void
  Bind(RenderGraph* graph, PipelineContext* ctx, uintptr_t set, uint16_t bindingPoint) const override;

  void
  SetArgument(DescriptorSetArgument& argument, uint16_t bindingPoint) const override {
    argument.Bind(bindingPoint, DescriptorType::IMAGE);
  }
};

struct StorageImageDesc final : public InputDesc, ImageDesc {
 public:
  ~StorageImageDesc() override = default;

  void
  Bind(RenderGraph* graph, PipelineContext* ctx, uintptr_t set, uint16_t bindingPoint) const override;

  void
  SetArgument(DescriptorSetArgument& argument, uint16_t bindingPoint) const override {
    argument.Bind(bindingPoint, DescriptorType::STORAGE_IMAGE);
  }
};

class RenderGraphGraphicsPass : public RenderGraphPass {
  friend class Marbas::RenderGraphGraphicsBuilder;
  friend class Marbas::RenderGraphGraphicsRegistry;

 public:
  RenderGraphGraphicsPass(StringView name, RHIFactory* rhiFactory);
  virtual ~RenderGraphGraphicsPass();

  void
  Initialize(RenderGraph* graph) final override;

  void
  AddSubResDesc(TextureAttachmentType type, const RenderGraphTextureHandler& handler, uint32_t baseLayer,
                uint32_t layerCount, uint32_t baseLevel, uint32_t levelCount);

  template <typename Desc, typename... Args>
  Desc*
  AddInputAttachment(Args&&... args) {
    auto desc = std::make_unique<Desc>(std::forward<Args>(args)...);
    auto* descPointer = desc.get();
    m_inputAttachment.push_back(std::move(desc));
    return descPointer;
  }

  void
  Submit(Semaphore* waitSemaphore, Semaphore* signedSemaphore, Fence* fence) override final {
    m_commandBuffer->Submit({&waitSemaphore, 1}, {&signedSemaphore, 1}, fence);
  }

 protected:
  std::vector<uintptr_t> m_pipelines;
  std::vector<GraphicsPipeLineCreateInfo> m_pipelineCreateInfos;
  GraphicsCommandBuffer* m_commandBuffer;

  FrameBuffer* m_framebuffer;
  uint32_t m_framebufferWidth;
  uint32_t m_framebufferHeight;
  uint32_t m_framebufferLayer;

  Vector<std::unique_ptr<InputDesc>> m_inputAttachment;  // the input from the last pass
  uintptr_t m_descriptorSet;                             // the descriptor set for input attachment

  Vector<std::unique_ptr<ImageDesc>> m_colorAttachment;
  std::unique_ptr<ImageDesc> m_depthAttachment;
  Vector<std::unique_ptr<ImageDesc>> m_resolveAttachment;
};

class RenderGraphComputePass : public RenderGraphPass {
  friend class Marbas::RenderGraphComputeBuilder;
  friend class Marbas::RenderGraphComputeRegistry;

 public:
  RenderGraphComputePass(std::string_view name, RHIFactory* rhiFactory);
  ~RenderGraphComputePass() override;

  void
  Initialize(RenderGraph* graph) final override;

  template <typename Desc, typename... Args>
  Desc*
  AddInputAttachment(Args&&... args) {
    auto desc = std::make_unique<Desc>(std::forward<Args>(args)...);
    auto* descPointer = desc.get();
    m_inputAttachment.push_back(std::move(desc));
    return descPointer;
  }

  void
  Submit(Semaphore* waitSemaphore, Semaphore* signedSemaphore, Fence* fence) override final {
    m_commandBuffer->Submit({&waitSemaphore, 1}, {&signedSemaphore, 1}, fence);
  }

 protected:
  std::vector<uintptr_t> m_pipelines;
  std::vector<ComputePipelineCreateInfo> m_pipelineCreateInfos;

  ComputeCommandBuffer* m_commandBuffer;
  Vector<std::unique_ptr<InputDesc>> m_inputAttachment;  // the input from the last pass
  uintptr_t m_descriptorSet;                             // the descriptor set for input attachment
};

template <typename T>
concept RenderGraphGraphicsLambdaPass = requires(T obj, RenderGraphGraphicsBuilder& builder) {
  obj(builder);
  { obj(builder) } -> std::invocable<RenderGraphGraphicsRegistry&, GraphicsCommandBuffer&>;
};

template <typename T>
concept RenderGraphComputeLambdaPass = requires(T obj, RenderGraphComputeBuilder& builder) {
  obj(builder);
  { obj(builder) } -> std::invocable<RenderGraphComputeRegistry&, ComputeCommandBuffer&>;
};

template <typename T>
concept RenderGraphComputeStructPass = requires(T obj) {
  requires std::invocable<decltype(&T::SetUp), T&, RenderGraphComputeBuilder&>;
  requires std::invocable<decltype(&T::Execute), T&, RenderGraphComputeRegistry&, ComputeCommandBuffer&>;
};

template <typename T>
concept RenderGraphGraphicsStructPass = requires(T obj) {
  requires std::invocable<decltype(&T::SetUp), T&, RenderGraphGraphicsBuilder&>;
  requires std::invocable<decltype(&T::Execute), T&, RenderGraphGraphicsRegistry&, GraphicsCommandBuffer&>;
};

class LambdaGraphicsRenderGraphPass final : public RenderGraphGraphicsPass {
  using Lambda = std::function<void(RenderGraphGraphicsRegistry&, GraphicsCommandBuffer&)>;
  using EnableFunc =
      std::variant<std::function<bool()>, std::function<bool(RenderGraphGraphicsRegistry&)>, std::monostate>;

 public:
  LambdaGraphicsRenderGraphPass(StringView name, RHIFactory* rhiFactory);
  virtual ~LambdaGraphicsRenderGraphPass() = default;

 public:
  void
  SetRecordCommand(const Lambda& command) {
    m_command = command;
  }

  void
  SetEnableFunc(const EnableFunc& func) {
    m_isEnable = func;
  }

 public:
  void
  Execute(RenderGraph* graph, void* userData) override;

  bool
  IsEnable(RenderGraph* graph, void* userData) override {
    if (std::holds_alternative<std::monostate>(m_isEnable)) {
      return true;
    } else if (std::get_if<0>(&m_isEnable)) {
      return std::get<0>(m_isEnable)();
    } else if (std::get_if<1>(&m_isEnable)) {
      RenderGraphGraphicsRegistry registry(graph, this, userData);
      return std::get<1>(m_isEnable)(registry);
    }
    return true;
  }

 private:
  Lambda m_command;
  EnableFunc m_isEnable = std::monostate();
};

template <details::RenderGraphGraphicsStructPass Pass, typename... Args>
class StructRenderGraphPass final : public RenderGraphGraphicsPass {
  define_has_member(IsEnable);

 public:
  StructRenderGraphPass(StringView name, RHIFactory* rhiFactory, Args&&... args)
      : RenderGraphGraphicsPass(name, rhiFactory), m_instance(std::forward<Args>(args)...) {}

  virtual ~StructRenderGraphPass() = default;

 public:
  void
  SetUp(RenderGraph* graph) {
    RenderGraphGraphicsBuilder builder(this, graph);
    m_instance.SetUp(builder);
  }

  void
  Execute(RenderGraph* graph, void* userData) override {
    RenderGraphGraphicsRegistry registry(graph, this, userData);
    m_instance.Execute(registry, *m_commandBuffer);
  }

  bool
  IsEnable(RenderGraph* graph, void* userData) override {
    if constexpr (has_member(Pass, IsEnable)) {
      if constexpr (std::is_invocable_v<decltype(&Pass::IsEnable), Pass&>) {
        return m_instance.IsEnable();
      } else if constexpr (std::is_invocable_v<decltype(&Pass::IsEnable), Pass&, RenderGraphGraphicsRegistry&>) {
        RenderGraphGraphicsRegistry registry(graph, this, userData);
        return m_instance.IsEnable(registry);
      }
    }
    return true;
  }

 private:
  Pass m_instance;
};

template <details::RenderGraphComputeStructPass Pass, typename... Args>
class StructRenderGraphComputePass final : public RenderGraphComputePass {
  define_has_member(IsEnable);

 public:
  StructRenderGraphComputePass(std::string_view name, RHIFactory* rhiFactory, Args&&... args)
      : RenderGraphComputePass(name, rhiFactory), m_instance(std::forward<Args>(args)...) {}
  ~StructRenderGraphComputePass() override = default;

 public:
  void
  SetUp(RenderGraph* graph) {
    RenderGraphComputeBuilder builder(this, graph);
    m_instance.SetUp(builder);
  }

  void
  Execute(RenderGraph* graph, void* userData) override {
    RenderGraphComputeRegistry registry(graph, this, userData);
    m_instance.Execute(registry, *m_commandBuffer);
  }

  bool
  IsEnable(RenderGraph* graph, void* userData) override {
    if constexpr (has_member(Pass, IsEnable)) {
      if constexpr (std::is_invocable_v<decltype(&Pass::IsEnable), Pass&>) {
        return m_instance.IsEnable();
      } else if constexpr (std::is_invocable_v<decltype(&Pass::IsEnable), Pass&, RenderGraphComputeRegistry&>) {
        RenderGraphComputeRegistry registry(graph, this, userData);
        return m_instance.IsEnable(registry);
      }
    }
    return true;
  }

 private:
  Pass m_instance;
};

class LambdaComputeRenderGraphPass final : public RenderGraphComputePass {
  using Lambda = std::function<void(RenderGraphComputeRegistry&, ComputeCommandBuffer&)>;
  using EnableFunc =
      std::variant<std::function<bool()>, std::function<bool(RenderGraphComputeRegistry&)>, std::monostate>;

 public:
  LambdaComputeRenderGraphPass(StringView name, RHIFactory* rhiFactory);
  virtual ~LambdaComputeRenderGraphPass() = default;

 public:
  void
  SetRecordCommand(const Lambda& command) {
    m_command = command;
  }

  void
  SetEnableFunc(const EnableFunc& func) {
    m_isEnable = func;
  }

 public:
  void
  Execute(RenderGraph* graph, void* userData) override;

  bool
  IsEnable(RenderGraph* graph, void* userData) override {
    if (std::holds_alternative<std::monostate>(m_isEnable)) {
      return true;
    } else if (std::get_if<0>(&m_isEnable)) {
      return std::get<0>(m_isEnable)();
    } else if (std::get_if<1>(&m_isEnable)) {
      RenderGraphComputeRegistry registry(graph, this, userData);
      return std::get<1>(m_isEnable)(registry);
    }
    return true;
  }

 private:
  Lambda m_command;
  EnableFunc m_isEnable = std::monostate();
};

}  // namespace details

}  // namespace Marbas
