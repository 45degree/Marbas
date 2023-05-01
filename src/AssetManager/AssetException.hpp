#pragma once

#include <exception>
#include <string_view>

#include "AssetPath.hpp"
#include "Common/Common.hpp"
#include "Uid.hpp"

namespace Marbas {

struct AssetException final : public std::exception {
 public:
  AssetException(StringView errMsg, const AssetPath& assertPath)
      : m_errMsg(FORMAT("an assert exception occur: {}, assert path is {}", errMsg, assertPath)) {}
  AssetException(StringView errMsg, Uid uid)
      : m_errMsg(FORMAT("an assert exception occur: {}, assert uid is {}", errMsg, uid)) {}

  ~AssetException() throw() {}

 public:
  const char*
  what() const throw() override {
    std::bad_alloc();
    return m_errMsg.data();
  }

 protected:
  String m_errMsg;
};

}  // namespace Marbas
