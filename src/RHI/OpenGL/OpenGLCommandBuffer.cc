#include "RHI/OpenGL/OpenGLCommandBuffer.hpp"

#include <glog/logging.h>

#include "RHI/Interface/Command.hpp"

namespace Marbas {

void
OpenGLCommandBuffer::BeginRecordCmd() {
  m_isBeginRecord = true;
}

void
OpenGLCommandBuffer::AddCommand(std::unique_ptr<ICommand>&& command) {
  if (!m_isBeginRecord) {
    LOG(ERROR) << "can't add command because the command buffer not begin to record cmd";
    return;
  }
  m_commands.push_back(std::move(command));
}

void
OpenGLCommandBuffer::EndRecordCmd() {
  m_isBeginRecord = false;
}

void
OpenGLCommandBuffer::SubmitCommand() {
  for (const auto& command : m_commands) {
    command->Execute();
  }

  for (auto iter = m_commands.crbegin(); iter != m_commands.crend(); iter++) {
    (*iter)->OnEndRenderPass();
  }
}

}  // namespace Marbas
