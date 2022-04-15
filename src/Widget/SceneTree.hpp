#ifndef MARBAS_WIDGET_SCENE_TREE
#define MARBAS_WIDGET_SCENE_TREE

#include "Widget/Widget.hpp"
#include "Widget/RenderImage.hpp"
#include "Core/Scene.hpp"

namespace Marbas {

class SceneTreeWidget : public Widget {
public:
    explicit SceneTreeWidget(RenderImage* renderImage):
        Widget("SceneTree"),
        m_renderImage(renderImage)
    {}
    ~SceneTreeWidget() override = default;

public:
    void SetScene(Scene* scene) {
        m_scene = scene;
    }

    void Draw() override;

private:
    void DrawNode(const SceneNode* node);

private:
    Scene* m_scene = nullptr;
    RenderImage* m_renderImage = nullptr;
};

}  // namespace Marbas

#endif
