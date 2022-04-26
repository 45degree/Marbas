#ifndef MARBAS_TOOL_UID_HPP
#define MARBAS_TOOL_UID_HPP

#include <cstdint>
#include <functional>
#include <random>

namespace Marbas {

class Uid {
    friend struct std::hash<Marbas::Uid>;
public:
    Uid();
    ~Uid() = default;

    Uid(const Uid& uid) = default;
    Uid& operator=(const Uid& uid) = default;

private:
    uint64_t m_uid;

private:
    static std::random_device s_randomDevice;
    static std::mt19937_64 s_gen64;

};

}  // namespace Marbas

namespace std {
    template<>
    struct hash<Marbas::Uid> {
        size_t operator()(const Marbas::Uid& uid) {
            return hash<uint64_t>()(uid.m_uid);
        }
    };
}

#endif
