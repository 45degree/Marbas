#include "RHI/OpenGL/OpenGLViewport.hpp"
#include "RHI/OpenGL/OpenGLRHICommon.hpp"

namespace Marbas {

void OpenGLViewport::UseViewport() const {
    glViewport(x, y, width, height);
}

}
