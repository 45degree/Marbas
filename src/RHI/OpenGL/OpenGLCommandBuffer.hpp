#pragma once

#include <variant>

#include "Common/Common.hpp"
#include "RHI/Interface/CommandBuffer.hpp"
#include "RHI/OpenGL/OpenGLCommand.hpp"
#include "RHI/OpenGL/OpenGLDescriptorSet.hpp"
#include "RHI/OpenGL/OpenGLPipeline.hpp"
#include "RHI/OpenGL/OpenGLRenderPass.hpp"

namespace Marbas {

struct Command {};

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
  BeginRecordCmd() override {
    m_isBeginRecord = true;
  }

  void
  BindDescriptorSet(const BindDescriptorSetInfo& info) override {
    DLOG_ASSERT(m_isBeginRecord);

    auto openglDescriptorSet = std::dynamic_pointer_cast<OpenGLDescriptorSet>(info.descriptorSet);
    OpenGLBindDescriptorSet cmd;
    cmd.SetDescriptor(openglDescriptorSet);
    cmd.SetDynamicDescriptorBufferPiece(info.bufferPiece);
    cmd.SetDescriptorLayout(info.layouts);
    m_commands.push_back(std::move(cmd));
  }

  void
  BeginRenderPass(const BeginRenderPassInfo& info) override {
    DLOG_ASSERT(m_isBeginRecord);

    m_currentRenderPass = std::static_pointer_cast<OpenGLRenderPass>(info.renderPass);
    m_currentFrameBuffer = std::dynamic_pointer_cast<OpenGLFrameBuffer>(info.frameBuffer);

    OpenGLBeginRenderPass cmd;
    cmd.SetRenderPass(m_currentRenderPass);
    cmd.SetClearColor(info.clearColor);
    cmd.SetFrameBuffer(info.frameBuffer);
    m_commands.push_back(std::move(cmd));
  }

  void
  BindPipeline(const std::shared_ptr<GraphicsPipeLine>& graphicsPipeline) override {
    DLOG_ASSERT(m_isBeginRecord);

    m_currentPipeline = std::static_pointer_cast<OpenGLGraphicsPipeline>(graphicsPipeline);

    OpenGLBindPipeline cmd;
    cmd.SetPipeLine(m_currentPipeline);
    m_commands.push_back(std::move(cmd));
  }

  void
  SetViewports(const Vector<ViewportInfo>& viewportInfos) override {
    OpenGLSetViewPorts cmd;
    cmd.SetViewPorts(viewportInfos);
    m_commands.push_back(std::move(cmd));
  }

  void
  BindVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override {
    DLOG_ASSERT(m_isBeginRecord);

    OpenGLBindVertexBuffer cmd;
    cmd.SetVertexBuffer(vertexBuffer);
    m_commands.push_back(std::move(cmd));
  }

  void
  BindIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override {
    DLOG_ASSERT(m_isBeginRecord);

    OpenGLBindIndexBuffer cmd;
    cmd.SetIndexBuffer(indexBuffer);
    m_commands.push_back(std::move(cmd));
  }

  void
  DrawIndex(int indexCount, int instanceCount) override {
    DLOG_ASSERT(m_isBeginRecord);

    OpenGLDrawIndex cmd;
    cmd.SetPipeline(m_currentPipeline);
    cmd.SetIndexCount(indexCount);
    cmd.SetInstanceCount(instanceCount);
    m_commands.push_back(std::move(cmd));
  }

  void
  DrawArray(int vertexCount, int instanceCount) override {
    DLOG_ASSERT(m_isBeginRecord);

    OpenGLDrawArray cmd;
    cmd.SetVertexCount(vertexCount);
    cmd.SetInstanceCount(instanceCount);
    cmd.SetPipeline(m_currentPipeline);
    m_commands.push_back(std::move(cmd));
  }

  void
  CopyImageToImage(const CopyImageToImageInfo& info) override {
    OpenGLCopyImageToImage cmd;
    cmd.SetSrcImage(info.srcTexture);
    cmd.SetDstImage(info.dstTexture);
    m_commands.push_back(std::move(cmd));
  }

  void
  EndRenderPass() override {
    DLOG_ASSERT(m_isBeginRecord);

    OpenGLEndRenderpass cmd;
    cmd.SetFrameBuffer(m_currentFrameBuffer);
    cmd.SetRenderPass(m_currentRenderPass);
    m_commands.push_back(std::move(cmd));
  }

  void
  EndRecordCmd() override {
    m_isBeginRecord = false;
  }

  void
  SubmitCommand() override {
    for (auto& command : m_commands) {
      std::visit([](auto& cmd) { cmd.Execute(); }, command);
    }

    for (auto iter = m_commands.crbegin(); iter != m_commands.crend(); iter++) {
      std::visit([](auto& cmd) { cmd.OnEndRenderPass(); }, *iter);
    }
  }

 private:
  std::shared_ptr<OpenGLFrameBuffer> m_currentFrameBuffer;
  std::shared_ptr<OpenGLRenderPass> m_currentRenderPass;
  std::shared_ptr<OpenGLGraphicsPipeline> m_currentPipeline;
  bool m_isBeginRecord = false;
  Vector<OpenGLCommandPoly_T> m_commands;
};

}  // namespace Marbas
