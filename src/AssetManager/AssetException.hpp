#pragma once

#include <exception>
#include <string_view>

#include "AssetPath.hpp"
#include "Common/Common.hpp"
#include "Uid.hpp"

namespace Marbas {

class AssetException : public std::exception {
 public:
  AssetException(StringView errMsg, const AssetPath& assertPath)
      : m_errMsg(FORMAT("an assert exception occur: {}, assert path is {}", errMsg, assertPath)) {}
  AssetException(StringView errMsg, Uid uid)
      : m_errMsg(FORMAT("an assert exception occur: {}, assert uid is {}", errMsg, uid)) {}

 public:
  const char*
  what() const noexcept override {
    return m_errMsg.data();
  }

 protected:
  String m_errMsg;
};

}  // namespace Marbas
