#include "AssetPath.hpp"

#include "AssetRegistry.hpp"

namespace Marbas {

static void
CheckPath(const std::string& path) {
  // check path format
  constexpr auto beginSize = std::string_view("res://").size();
  if (path.size() < beginSize) {
    throw std::out_of_range("assert path must begin with res://");
  }
  if (path.substr(0, beginSize) != "res://") {
    throw std::out_of_range("assert path must begin with res://");
  }
}

AssetPath::AssetPath() : m_path("res://") {}

AssetPath::AssetPath(const std::string& path) : m_path(path) {
  CheckPath(m_path);
  std::replace(m_path.begin(), m_path.end(), '\\', '/');
}

AssetPath::AssetPath(const char* path) : m_path(path) {
  CheckPath(m_path);
  std::replace(m_path.begin(), m_path.end(), '\\', '/');
}

AssetPath::AssetPath(const std::string_view path) : m_path(path) {
  CheckPath(m_path);
  std::replace(m_path.begin(), m_path.end(), '\\', '/');
}

AssetPath ::AssetPath(const AssetPath& obj) : m_path(obj.m_path) {}

AssetPath&
AssetPath::operator=(const AssetPath& obj) {
  m_path = obj.m_path;
  return *this;
}

Path
AssetPath::GetAbsolutePath() const {
  constexpr auto beginSize = std::string_view("res://").size();
  const auto* registry = AssetRegistry::GetInstance();
  auto& projectDir = registry->GetProjectDir();

  auto filename = (projectDir / m_path.substr(beginSize)).string();
#ifdef _WIN32
  std::replace(filename.begin(), filename.end(), '/', '\\');
#elif __linux__
  std::replace(filename.begin(), filename.end(), '\\', '/');
#endif

  return filename;
}

String
AssetPath::Stem() const {
  constexpr auto beginSize = std::string_view("res://").size();
  return Path(m_path.substr(beginSize, m_path.size() - beginSize)).stem().string();
}

bool
AssetPath::ConvertToAssetPath(const Path& path, AssetPath& assetPath) {
  auto registry = AssetRegistry::GetInstance();
  auto& projectDir = registry->GetProjectDir();

  if (!AssetPath::IsSubpath(path, projectDir)) return false;
  assetPath = "res://" + std::filesystem::relative(path, projectDir).string();

  return true;
}

}  // namespace Marbas
