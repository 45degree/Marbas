#ifndef MARBAS_RHI_INTERFACE_DRAWCOLLECTION_H
#define MARBAS_RHI_INTERFACE_DRAWCOLLECTION_H

#include "Common.h"
#include "RHI/Interface/DrawUnit.h"

#include <unordered_set>

namespace Marbas {

class DrawCollection {
public:
    DrawCollection() = default;
    virtual ~DrawCollection() = default;

public:
    void AddDrawUnit(DrawUnit* drawUnit) {
        m_drawUnits.insert(drawUnit);
    }

    virtual void Draw() = 0;

public:
    std::unordered_set<DrawUnit*> m_drawUnits;
};

}  // namespace Marbas


#endif
