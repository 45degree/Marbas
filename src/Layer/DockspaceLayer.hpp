#ifndef MARBARS_LAYER_DOCKSPACE_LAYER_H
#define MARBARS_LAYER_DOCKSPACE_LAYER_H

#include "Layer/LayerBase.hpp"

namespace Marbas {

class DockspaceLayer : public LayerBase {
public:
    explicit DockspaceLayer(const Window* window) :
        LayerBase(window)
    {}
    ~DockspaceLayer() override;

public:
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnBegin() override;
    void OnEnd() override;
};

}  // namespace Marbas

#endif
