#ifndef MARBAS_RESOURCE_RESOURCE_BASE_HPP
#define MARBAS_RESOURCE_RESOURCE_BASE_HPP

namespace Marbas {

enum class ResourceType {
    TEXTURE2D,
    SHADER,
};

class ResourceBase {
public:
    explicit ResourceBase(ResourceType type, int id = 0):
        m_type(type),
        m_id(id)
    {}

    virtual ~ResourceBase() = default;

protected:
    int m_id;
    ResourceType m_type;
};

}  // namespace Marbas

#endif
