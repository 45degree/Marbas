#pragma once

#include "Common/Common.hpp"
#include "RHI/Interface/CommandBuffer.hpp"

namespace Marbas {

class OpenGLCommandBuffer final : public CommandBuffer {
 public:
  OpenGLCommandBuffer() = default;
  ~OpenGLCommandBuffer() = default;
  OpenGLCommandBuffer(const OpenGLCommandBuffer&) = delete;

 public:
  void
  Clear() override {
    m_commands.clear();
  }

  void
  BeginRecordCmd() override;

  void
  AddCommand(std::unique_ptr<ICommand>&& command) override;

  void
  EndRecordCmd() override;

  void
  SubmitCommand() override;

 private:
  bool m_isBeginRecord = false;
  Vector<std::unique_ptr<ICommand>> m_commands;
};

}  // namespace Marbas
