#ifndef MARBAS_WIDGET_SCENE_TREE
#define MARBAS_WIDGET_SCENE_TREE

#include "Core/Scene.hpp"
#include "Widget/Widget.hpp"

namespace Marbas {

class SceneTreeWidget : public Widget {
 public:
  explicit SceneTreeWidget() : Widget("SceneTree") {}
  ~SceneTreeWidget() override = default;

 public:
  void Draw() override;

  void AddSelectMeshWidget(IChangeMeshAble* widget) { m_selectMeshWidgets.push_back(widget); }

 private:
  void DrawNode(const SceneNode* node);

 private:
  Vector<IChangeMeshAble*> m_selectMeshWidgets;
};

}  // namespace Marbas

#endif
