#pragma once

namespace Marbas {

/**
 * the tag for light
 */

struct NewLightTag {};

struct UpdateLightTag {};

struct DeleteLightTag {};

/**
 * tag for renderable
 */

/**
 * @class RenderComponent
 * @brief a tag means the node need to be rendered
 */
struct RenderComponent {};

}  // namespace Marbas
