#ifndef MARBAS_WIDGET_SCENE_TREE
#define MARBAS_WIDGET_SCENE_TREE

#include "Widget/Widget.h"
#include "Core/Scene.h"

namespace Marbas {

class SceneTreeWidget : public Widget {
public:
    SceneTreeWidget(): Widget("SceneTree") {}
    ~SceneTreeWidget() override = default;

public:
    void SetScnen(Scene* scene) {
        m_scene = scene;
    }

    void Draw() override;

private:
    Scene* m_scene;
};

}  // namespace Marbas

#endif
