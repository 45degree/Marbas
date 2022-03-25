#ifndef MARBARS_RHI_INTERFACE_VIEWPORT_H
#define MARBARS_RHI_INTERFACE_VIEWPORT_H

#include "Common.h"
#include <tuple>

namespace Marbas {

class Viewport {
public:
    Viewport() = default;
    virtual ~Viewport() = default;

public:
    void SetViewport(int x, int y, int width, int height) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    virtual void UseViewport() const = 0;

    [[nodiscard]] std::tuple<int, int, int, int> GetViewport() const noexcept {
        return std::make_tuple(x, y, width, height);
    }

protected:
    int x, y;
    int width, height;
};

}  // namespace Marbas

#endif
