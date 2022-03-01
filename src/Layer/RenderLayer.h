#ifndef MARBARS_LAYER_RENDER_LAYER_H
#define MARBARS_LAYER_RENDER_LAYER_H

#include "Layer/Layer.h"
#include "Renderer/Interface/VertexBuffer.h"
#include "Renderer/Interface/VertexArray.h"
#include "Renderer/Interface/IndexBuffer.h"
#include "Renderer/Interface/Shader.h"
#include "Renderer/Interface/DrawCall.h"

namespace Marbas {

class RenderLayer : public Layer{
public:
    RenderLayer(): Layer("RenderLayer") {}
    ~RenderLayer() override;

public:
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;

private:
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<VertexArray> vertexArray;
    std::unique_ptr<IndexBuffer> indexBuffer;
    std::unique_ptr<Shader> vertexShader;
    std::unique_ptr<Shader> fragmentShader;
    std::unique_ptr<DrawCall> drawCall;
};

}  // namespace Marbas

#endif
