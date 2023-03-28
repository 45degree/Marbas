#pragma once

#include "AssetPath.hpp"
#include "Singleton.hpp"
#include "Uid.hpp"

namespace Marbas {

class AssetRegistryImpl final {
 public:
  void
  SetProjectDir(const Path& projectDir);

  template <class Archive>
  void
  serialize(Archive& ar) {
    ar(m_assertUid);
  }

  Uid
  CreateOrFindAssertUid(const AssetPath& path);

  bool
  Existed(const AssetPath& path) {
    std::lock_guard lock(m_mutex);
    return m_assertUid.find(path) != m_assertUid.end();
  }

  size_t
  AssertCount() const {
    std::lock_guard lock(m_mutex);
    return m_assertUid.size();
  }

  void
  SaveAllAssert();

  Path&
  GetProjectDir() {
    return m_projectDir;
  }

  const Path&
  GetProjectDir() const {
    return m_projectDir;
  }

  Path
  GetAssertAbsolutePath(Uid uid) {
    return m_assertImportDir / (std::to_string(uid) + ".data");
  }

 private:
  mutable std::mutex m_mutex;
  HashMap<AssetPath, Uid> m_assertUid;

  Path m_projectDir;
  Path m_assertImportDir;
  Path m_assertUidDB;
};

using AssetRegistry = Singleton<AssetRegistryImpl>;
using AssetRegistryType = AssetRegistryImpl;

}  // namespace Marbas
