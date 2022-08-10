#pragma once

#include "RHI/Interface/CommandFactory.hpp"

namespace Marbas {

class OpenGLCommandFactory final : public CommandFactory {
 public:
  std::unique_ptr<CommandBuffer>
  CreateCommandBuffer() override;

  std::unique_ptr<BindDescriptorSet>
  CreateBindDescriptorSetCMD() override;

  std::unique_ptr<BindDynamicDescriptorSet>
  CreateBindDynamicDescriptorSetCMD() override;

  std::unique_ptr<BeginRenderPass>
  CreateBeginRenderPassCMD() override;

  std::unique_ptr<EndRenderPass>
  CreateEndRenderPassCMD() override;

  std::unique_ptr<BindPipeline>
  CreateBindPipelineCMD() override;

  std::unique_ptr<BindVertexBuffer>
  CreateBindVertexBufferCMD() override;

  std::unique_ptr<BindIndexBuffer>
  CreateBindIndexBufferCMD() override;

  std::unique_ptr<DrawIndex>
  CreateDrawIndexCMD(const std::shared_ptr<GraphicsPipeLine>& pipeline) override;

  std::unique_ptr<DrawArray>
  CreateDrawArrayCMD(const std::shared_ptr<GraphicsPipeLine>& pipeline) override;
};

}  // namespace Marbas
