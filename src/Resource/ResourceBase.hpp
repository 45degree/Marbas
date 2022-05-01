#ifndef MARBAS_RESOURCE_RESOURCE_BASE_HPP
#define MARBAS_RESOURCE_RESOURCE_BASE_HPP

#include "Common.hpp"
#include "Tool/Uid.hpp"

namespace Marbas {

class ResourceBase {
protected:
    ResourceBase() = default;

public:
    virtual ~ResourceBase() = default;

public:
    [[nodiscard]] Uid GetUid() const noexcept {
        return m_id;
    }

protected:
    Uid m_id;
    mutable bool m_isLoad = false;
};

}  // namespace Marbas

#endif
