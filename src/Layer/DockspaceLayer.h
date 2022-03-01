#ifndef MARBARS_LAYER_DOCKSPACE_LAYER_H
#define MARBARS_LAYER_DOCKSPACE_LAYER_H

#include "Layer/Layer.h"

namespace Marbas {

class DockspaceLayer : public Layer {
public:
    DockspaceLayer() : Layer("DockspaceLayer") {}
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
