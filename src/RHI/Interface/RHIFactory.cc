#include "RHI/Interface/RHIFactory.hpp"

#include <glog/logging.h>

#include <mutex>

#include "Common/Common.hpp"
#include "RHI/OpenGL/OpenGLRHIFactory.hpp"
#include "RHI/Vulkan/VulkanRHIFactory.hpp"

namespace Marbas {

std::unique_ptr<RHIFactory> RHIFactory::m_rhiFactory = nullptr;

RHIFactory*
RHIFactory::GetInstance(const RendererType& rendererType) {
  static std::once_flag flag;
  std::call_once(flag, [&]() {
    if (rendererType == RendererType::OPENGL) {
      m_rhiFactory.reset(new OpenGLRHIFactory());
    } else if (rendererType == RendererType::VULKAN) {
      // TODO(45degree): change to vulkan api
      m_rhiFactory.reset(new VulkanRHIFactory());
    } else {
      LOG(ERROR) << "can't create the renderer api";
    }
  });
  return m_rhiFactory.get();
}

}  // namespace Marbas
