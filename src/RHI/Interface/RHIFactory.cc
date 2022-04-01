#include "RHI/Interface/RHIFactory.h"
#include "RHI/OpenGL/OpenGLRHIFactory.h"
#include "Common.h"

#include <glog/logging.h>
#include <mutex>

namespace Marbas {

std::unique_ptr<RHIFactory> RHIFactory::m_rhiFactory = nullptr;

RHIFactory* RHIFactory::GetInstance(const RendererType& rendererType) {
    static std::once_flag flag;
    std::call_once(flag, [&]() {
        if(rendererType == RendererType::OPENGL) {
            m_rhiFactory.reset(new OpenGLRHIFactory());
        }
        else if (rendererType == RendererType::VULKAN) {
            // TODO(45degree): change to vulkan api
            m_rhiFactory.reset(new OpenGLRHIFactory());
        }
        else {
            LOG(ERROR) << "can't create the renderer api";
        }
    });
    return m_rhiFactory.get();
}


}  // namespace Marbas
