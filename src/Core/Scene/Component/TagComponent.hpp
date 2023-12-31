#pragma once

namespace Marbas {

/**
 * tag for renderable
 */

/**
 * @class Renderable
 * @brief a tag means the node can be rendered
 */
struct RenderableTag {};

/**
 * @class RenderPreparedTag
 * @brief the node has prepared all the data which is needed by renderer
 *        the entity has this tag means it will be rendered in current frame
 *
 */
struct RenderableMeshTag {};

/**
 * vxgi probe
 */
struct NewVXGIProbeTag {};

struct VXGIProbeUpdateTag {};

/**
 * editor component
 */
struct EditorComponentTag {};

}  // namespace Marbas
