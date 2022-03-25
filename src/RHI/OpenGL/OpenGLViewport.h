#ifndef MARBARS_RHI_OPENGL_VIEWPORT_H
#define MARBARS_RHI_OPENGL_VIEWPORT_H

#include "RHI/Interface/Viewport.h"

namespace Marbas {

class OpenGLViewport : public Viewport {
public:
    OpenGLViewport() = default;
    ~OpenGLViewport() override = default;

public:
    void UseViewport() const override {
        glViewport(x, y, width, height);
    }
};

}  // namespace Marbas

#endif
