
#include "RHI/OpenGL/OpenGLCommandFactory.hpp"

#include "RHI/OpenGL/OpenGLCommand.hpp"
#include "RHI/OpenGL/OpenGLCommandBuffer.hpp"

namespace Marbas {

std::unique_ptr<CommandBuffer>
OpenGLCommandFactory::CreateCommandBuffer() {
  return std::make_unique<OpenGLCommandBuffer>();
}

std::unique_ptr<BindDescriptorSet>
OpenGLCommandFactory::CreateBindDescriptorSetCMD() {
  return std::make_unique<OpenGLBindDescriptorSet>();
}

std::unique_ptr<BindDynamicDescriptorSet>
OpenGLCommandFactory::CreateBindDynamicDescriptorSetCMD() {
  return std::make_unique<OpenGLBindDynamicDescriptorSet>();
}

std::unique_ptr<BeginRenderPass>
OpenGLCommandFactory::CreateBeginRenderPassCMD() {
  return std::make_unique<OpenGLBeginRenderPass>();
}

std::unique_ptr<EndRenderPass>
OpenGLCommandFactory::CreateEndRenderPassCMD() {
  return std::make_unique<OpenGLEndRenderpass>();
}

std::unique_ptr<BindPipeline>
OpenGLCommandFactory::CreateBindPipelineCMD() {
  return std::make_unique<OpenGLBindPipeline>();
}

std::unique_ptr<BindVertexBuffer>
OpenGLCommandFactory::CreateBindVertexBufferCMD() {
  return std::make_unique<OpenGLBindVertexBuffer>();
}

std::unique_ptr<BindIndexBuffer>
OpenGLCommandFactory::CreateBindIndexBufferCMD() {
  return std::make_unique<OpenGLBindIndexBuffer>();
}

std::unique_ptr<DrawIndex>
OpenGLCommandFactory::CreateDrawIndexCMD() {
  return std::make_unique<OpenGLDrawIndex>();
}

std::unique_ptr<DrawArray>
OpenGLCommandFactory::CreateDrawArrayCMD() {
  return std::make_unique<OpenGLDrawArray>();
}

}  // namespace Marbas
