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
    void SetScene(Scene* scene) {
        m_scene = scene;
    }

    void Draw() override;

private:
    Scene* m_scene = nullptr;
};

}  // namespace Marbas

#endif
