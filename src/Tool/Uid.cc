#include "Tool/Uid.hpp"

#include <random>

namespace Marbas {

std::random_device Uid::s_randomDevice;
std::mt19937_64 Uid::s_gen64(s_randomDevice());

Uid::Uid() :
    m_uid(s_gen64())
{
}

}  // namespace Marbas
