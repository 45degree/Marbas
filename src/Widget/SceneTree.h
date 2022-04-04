#ifndef MARBAS_WIDGET_SCENE_TREE
#define MARBAS_WIDGET_SCENE_TREE

#include "Widget/Widget.h"

namespace Marbas {

class SceneTreeWidget : public Widget {
public:
    SceneTreeWidget(): Widget("SceneTree") {}
    ~SceneTreeWidget() override = default;

public:
    void Draw() override;

};

}  // namespace Marbas

#endif
