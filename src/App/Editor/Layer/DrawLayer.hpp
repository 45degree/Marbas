#include "App/Editor/Widget/Widget.hpp"
#include "Core/Layer/ImguiLayer.hpp"
#include "Core/Layer/LayerBase.hpp"
#include "Resource/ResourceManager.hpp"

namespace Marbas {

class DrawLayer final : public LayerBase {
 public:
  explicit DrawLayer(const std::weak_ptr<Window>& windows,
                     const std::shared_ptr<ImguiLayer>& imguiLayer)
      : LayerBase(windows), m_imguiLayer(imguiLayer){};
  ~DrawLayer() override {}

 public:
  void
  OnAttach() override;

  void
  OnUpdate() override;

  void
  AddWidget(std::unique_ptr<Widget>&& widget) noexcept {
    m_widgetsMap[widget->GetWidgetName()] = widget.get();
    widgets.push_back(std::move(widget));
  }

  void
  DrawMenuBar();

 private:
  Vector<std::unique_ptr<Widget>> widgets;
  std::shared_ptr<ImguiLayer> m_imguiLayer;

  std::unordered_map<String, Widget*> m_widgetsMap;
};

}  // namespace Marbas
