#ifndef MARBARS_RHI_OPENGL_VIEWPORT_H
#define MARBARS_RHI_OPENGL_VIEWPORT_H

#include "RHI/Interface/Viewport.hpp"

namespace Marbas {

class OpenGLViewport : public Viewport {
public:
    OpenGLViewport() = default;
    ~OpenGLViewport() override = default;

public:
    void UseViewport() const override;
};

}  // namespace Marbas

#endif
