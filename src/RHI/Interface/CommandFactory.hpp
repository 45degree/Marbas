#pragma once

#include <memory>

#include "RHI/Interface/Command.hpp"
#include "RHI/Interface/CommandBuffer.hpp"

namespace Marbas {

class CommandFactory {
 public:
  virtual std::unique_ptr<CommandBuffer>
  CreateCommandBuffer() = 0;

  virtual std::unique_ptr<CopyImageToImage>
  CreateCopyImageToImageCMD() = 0;

  virtual std::unique_ptr<BindDescriptorSet>
  CreateBindDescriptorSetCMD() = 0;

  // virtual std::unique_ptr<BindDynamicDescriptorSet>
  // CreateBindDynamicDescriptorSetCMD() = 0;

  virtual std::unique_ptr<BeginRenderPass>
  CreateBeginRenderPassCMD() = 0;

  virtual std::unique_ptr<EndRenderPass>
  CreateEndRenderPassCMD() = 0;

  virtual std::unique_ptr<BindPipeline>
  CreateBindPipelineCMD() = 0;

  virtual std::unique_ptr<BindVertexBuffer>
  CreateBindVertexBufferCMD() = 0;

  virtual std::unique_ptr<BindIndexBuffer>
  CreateBindIndexBufferCMD() = 0;

  /**
   * @brief create draw index cmd
   *
   * @param pipeline the graphics pipeline
   *
   * @note this command is comming from the vulkan, and in vulkan, this command can also draw the
   *       instance, but in opengl, we use the glDrawElementsInstanced not glDrawElements to draw
   *       the instance, so, in ordered to judge whether to draw the instance in opengl, we need to
   *       pass the pipeline to the command.
   */
  virtual std::unique_ptr<DrawIndex>
  CreateDrawIndexCMD(const std::shared_ptr<GraphicsPipeLine>& pipeline) = 0;

  /**
   * @brief create draw array cmd
   *
   * @param pipeline the graphics pipeline
   *
   * @see CreateDrawIndexCMD
   */
  virtual std::unique_ptr<DrawArray>
  CreateDrawArrayCMD(const std::shared_ptr<GraphicsPipeLine>& pipeline) = 0;
};

}  // namespace Marbas
