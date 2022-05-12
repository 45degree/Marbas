#ifndef MARBAS_WIDGET_SCENE_TREE
#define MARBAS_WIDGET_SCENE_TREE

#include "Core/Scene.hpp"
#include "Widget/FileDialog.hpp"
#include "Widget/Widget.hpp"

namespace Marbas {

class SceneTreeWidget : public Widget {
 public:
  explicit SceneTreeWidget();
  ~SceneTreeWidget() override = default;

 public:
  void Draw() override;

  void AddSelectMeshWidget(IChangeMeshAble* widget) { m_selectMeshWidgets.push_back(widget); }

 private:
  void DrawNode(const SceneNode* node);

 private:
  SceneNode* m_selectedNode = nullptr;
  Vector<IChangeMeshAble*> m_selectMeshWidgets;
  std::unique_ptr<FileDialog> m_modeladdDialog = nullptr;
};

}  // namespace Marbas

#endif
