#pragma once

#include "Core/Scene/Component/EnvironmentComponent.hpp"
#include "Resource/ResourceManager.hpp"

namespace Marbas {

template <typename Component>
void
LoadComponentImpl(Component& component, const ResourceManager* resourceMgr) {
  DLOG_ASSERT(false);
}

template <>
void
LoadComponentImpl<EnvironmentComponent>(EnvironmentComponent& component,
                                        const ResourceManager* resourceMgr);

}  // namespace Marbas
