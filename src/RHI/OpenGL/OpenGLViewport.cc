#include "RHI/OpenGL/OpenGLViewport.h"
#include "RHI/OpenGL/OpenGLRHICommon.h"

namespace Marbas {

void OpenGLViewport::UseViewport() const {
    glViewport(x, y, width, height);
}

}
