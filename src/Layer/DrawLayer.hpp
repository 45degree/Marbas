#ifndef MARBARS_LAYER_DRAW_H
#define MARBARS_LAYER_DRAW_H

#include "Layer/LayerBase.hpp"
#include "Resource/ResourceManager.hpp"
#include "Widget/Widget.hpp"
#include "Widget/FileDialog.hpp"

namespace Marbas {

class DrawLayer : public LayerBase {
public:
    DrawLayer(const Window* windows, ResourceManager* resourceManager);
    ~DrawLayer() override;

public:
    void OnAttach() override;

    void OnUpdate() override;

    void AddWidget(std::unique_ptr<Widget>&& widget) noexcept {
        m_widgetsMap[widget->GetWidgetName()] = widget.get();
        widgets.push_back(std::move(widget));
    }

    void DrawMenuBar();

private:
    Vector<std::unique_ptr<Widget>> widgets;

    std::unordered_map<String, Widget*> m_widgetsMap;

    std::unique_ptr<FileDialog> m_fileDialog;

    std::unique_ptr<FileDialog> m_sceneFileDialog;

    ResourceManager* m_resourceManager;
};

}  // namespace Marbas

#endif
