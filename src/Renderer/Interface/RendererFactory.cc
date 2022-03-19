#include "Renderer/Interface/RendererFactory.h"
#include "Renderer/OpenGL/OpenGLRendererFactory.h"
#include "Common.h"

#include <mutex>

namespace Marbas {

std::unique_ptr<RendererFactory> RendererFactory::m_rendererFactory;

RendererFactory* RendererFactory::GetInstance(const RendererType& rendererType) {
    static std::once_flag flag;
    std::call_once(flag, [&]() {
        if(rendererType == RendererType::OPENGL) {
            m_rendererFactory.reset(new OpenGLRendererFactory());
        }
        else if (rendererType == RendererType::VULKAN) {
            // TODO(45degree): change to vulkan api
            m_rendererFactory.reset(new OpenGLRendererFactory());
        }
        else {
            LOG(ERROR) << "can't create the renderer api";
        }
    });
    return m_rendererFactory.get();
}


}  // namespace Marbas
