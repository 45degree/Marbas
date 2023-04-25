#include "AssetRegistry.hpp"

#include <cereal/archives/binary.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <fstream>

namespace Marbas {

void
AssetRegistryImpl::SetProjectDir(const Path& projectDir) {
  std::lock_guard lock(m_mutex);

  // clear all assert path in current instance
  m_assertUid.clear();

  m_projectDir = projectDir;
  m_assertImportDir = projectDir / ".import";
  m_assertUidDB = projectDir / "assert.db";

  if (!std::filesystem::exists(m_projectDir)) {
    std::filesystem::create_directory(m_projectDir);
  }

  if (!std::filesystem::exists(m_assertImportDir)) {
    std::filesystem::create_directory(m_assertImportDir);
  }

  if (!std::filesystem::exists(m_assertUidDB)) {
    // create a empty db file
    std::ofstream os(m_assertUidDB, std::ios::binary | std::ios::out);
    cereal::BinaryOutputArchive archive(os);
    archive(*this);
  }

  std::ifstream os(m_assertUidDB, std::ios::binary | std::ios::in);
  cereal::BinaryInputArchive archive(os);
  archive(*this);
}

void
AssetRegistryImpl::SaveAllAssert() {
  std::lock_guard lock(m_mutex);
  std::ofstream os(m_assertUidDB, std::ios::binary | std::ios::out);
  cereal::BinaryOutputArchive archive(os);
  archive(*this);
}

Uid
AssetRegistryImpl::CreateOrFindAssertUid(const AssetPath& path) {
  std::lock_guard lock(m_mutex);

  // get or create uid for path
  if (m_assertUid.find(path) != m_assertUid.cend()) {
    return m_assertUid.at(path);
  }

  m_assertUid[path] = Uid();

  // save the asset uid
  std::ofstream os(m_assertUidDB, std::ios::binary|std::ios::out);
  cereal::BinaryOutputArchive archive(os);
  archive(*this);

  return m_assertUid.at(path);
}

}  // namespace Marbas
