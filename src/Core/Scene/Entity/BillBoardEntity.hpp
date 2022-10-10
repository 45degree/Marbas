#pragma once

#include "Tool/Uid.hpp"
#include "entt/entity/fwd.hpp"

namespace Marbas {

using BillBoardEntity = entt::entity;

class BillBoardPolicy {
 public:
  BillBoardPolicy() = delete;
  BillBoardPolicy(const BillBoardPolicy&) = delete;
  BillBoardPolicy&
  operator=(const BillBoardPolicy&) = delete;

  static BillBoardEntity
  Create(entt::registry& registry, Uid texture2DResourceId);
};

}  // namespace Marbas
