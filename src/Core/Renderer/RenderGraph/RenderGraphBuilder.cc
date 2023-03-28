#include "RenderGraphBuilder.hpp"

#include <fstream>

#include "RenderGraph.hpp"

namespace Marbas {

void
RenderGraphPipelineCreateInfo::AddShader(ShaderType shaderType, const Path& shaderPath) {
  std::fstream file;
  file.open(shaderPath.string().c_str(), std::ios::in | std::ios::binary);
  std::vector<char> binaryCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

  ShaderStageCreateInfo createInfo;
  createInfo.code = binaryCode;
  createInfo.stage = shaderType;
  createInfo.interName = "main";
  m_shaders.push_back(createInfo);
}

RenderGraphGraphicsBuilder::RenderGraphGraphicsBuilder(details::RenderGraphGraphicsPass* pass, RenderGraph* graph)
    : m_graph(graph), m_pass(pass) {}

void
RenderGraphGraphicsBuilder::WriteTexture(const RenderGraphTextureHandler& handler, TextureAttachmentType type,
                                         int baseLayer, int LayerCount, int baseLevel, int levelCount) {
  auto& res = m_graph->m_resourceManager->m_graphTexture[handler.index];
  res.inputs.push_back(m_pass);
  m_pass->outputs.push_back(&res);
  m_pass->AddSubResDesc(type, handler, baseLayer, LayerCount, baseLevel, levelCount);
}

void
RenderGraphGraphicsBuilder::ReadTexture(const RenderGraphTextureHandler& handler) {
  auto& res = m_graph->m_resourceManager->m_graphTexture[handler.index];
  res.outputs.push_back(m_pass);
  m_pass->inputs.push_back(&res);
}

void
RenderGraphGraphicsBuilder::SetFramebufferSize(uint32_t width, uint32_t height, uint32_t layer) {
  m_pass->m_framebufferWidth = width;
  m_pass->m_framebufferHeight = height;
  m_pass->m_framebufferLayer = layer;
}

void
RenderGraphGraphicsBuilder::SetPipelineInfo(const RenderGraphPipelineCreateInfo& createInfo) {
  m_pass->m_pipelineCreateInfo = createInfo;
}

}  // namespace Marbas
