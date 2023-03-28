#include "Uid.hpp"

#include <random>

namespace Marbas {

std::random_device Uid::s_randomDevice;
std::mt19937_64 Uid::s_gen64(s_randomDevice());

Uid::Uid() : m_uid(s_gen64()) {}

// Uid::Uid(uint64_t uid) : m_uid(uid) {}

}  // namespace Marbas

namespace std {

std::string
to_string(Marbas::Uid uid) {
  return std::to_string(uid.m_uid);
}
}  // namespace std
