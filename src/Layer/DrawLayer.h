#ifndef MARBARS_LAYER_DRAW_H
#define MARBARS_LAYER_DRAW_H

#include "Layer/LayerBase.h"
#include "Widget/Widget.h"
#include "Widget/FileDialog.h"

namespace Marbas {

class DrawLayer : public LayerBase {
public:
    explicit DrawLayer(const Window* windows);
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
};

}  // namespace Marbas

#endif
