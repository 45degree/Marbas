#ifndef MARBAS_TOOL_UID_HPP
#define MARBAS_TOOL_UID_HPP

#include <cstdint>
#include <functional>
#include <random>

#include <folly/Format.h>

namespace Marbas {

class Uid {
    friend struct std::hash<Marbas::Uid>;
    friend class folly::FormatValue<Marbas::Uid>;
public:
    Uid();
    ~Uid() = default;

    Uid(const Uid& uid) = default;
    Uid& operator=(const Uid& uid) = default;

    bool operator==(const Uid& uid) const {
        return m_uid == uid.m_uid;
    }

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
        size_t operator()(const Marbas::Uid& uid) const {
            return hash<uint64_t>()(uid.m_uid);
        }
    };
}

namespace folly {

template <>
class FormatValue<Marbas::Uid> {
public:
    explicit FormatValue(const Marbas::Uid& val) : val_(val) {}

    template <class FormatCallback>
    void format(FormatArg& arg, FormatCallback& cb) const {
        FormatValue<uint64_t>(val_.m_uid).format(arg, cb);
    }

private:
    const Marbas::Uid& val_;
};

}  // namespace folly


#endif
