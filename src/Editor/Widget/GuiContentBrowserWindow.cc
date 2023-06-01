#include "GuiContentBrowserWindow.hpp"

#include <imgui.h>
#include <stb_image.h>

#include <cstring>

#include "AssetManager/AssetRegistry.hpp"
#include "CommonName.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

namespace Marbas::Gui {
static std::optional<Path> s_currentPath;
static Path s_fileIconPath = "./assert/icon/FileIcon.png";
static Path s_floderIconPath = "./assert/icon/DirectoryIcon.png";
static Path s_backIconPath = "./assert/icon/back.png";

static ImTextureID
LoadToImGui(const Path& path, RHIFactory* rhiFactory) {
  int width, height, channel;
  void* data = stbi_load(path.string().c_str(), &width, &height, &channel, STBI_rgb_alpha);

  auto imguiContext = rhiFactory->GetImguiContext();
  auto bufContext = rhiFactory->GetBufferContext();
  bufContext->CreateImage(ImageCreateInfo{});

  ImageCreateInfo imageCreateInfo{
      .width = static_cast<uint32_t>(width),
      .height = static_cast<uint32_t>(height),
      .format = ImageFormat::RGBA,
      .sampleCount = SampleCount::BIT1,
      .mipMapLevel = 1,
      .usage = ImageUsageFlags::SHADER_READ,
      .imageDesc = Image2DDesc(),
  };
  auto image = bufContext->CreateImage(imageCreateInfo);

  bufContext->UpdateImage(UpdateImageInfo{
      .image = image,
      .level = 0,
      .xOffset = 0,
      .yOffset = 0,
      .zOffset = 0,
      .width = width,
      .height = height,
      .depth = 1,
      .data = data,
      .dataSize = static_cast<uint32_t>(height * width * 4 * sizeof(unsigned char)),
  });

  stbi_image_free(data);

  auto imageView = bufContext->CreateImageView(ImageViewCreateInfo{
      .image = image,
      .type = ImageViewType::TEXTURE2D,
      .baseLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  });

  return imguiContext->CreateImGuiImage(imageView);
}

GuiContentBrowserWindow::GuiContentBrowserWindow(RHIFactory* rhiFactory)
    : GuiWindow("ContentBrowser"), m_rhiFactory(rhiFactory) {
  auto imguiContext = m_rhiFactory->GetImguiContext();
  auto bufContext = m_rhiFactory->GetBufferContext();
  m_fileIcon = LoadToImGui(s_fileIconPath, m_rhiFactory);
  m_floderIcon = LoadToImGui(s_floderIconPath, m_rhiFactory);
  m_backIcon = LoadToImGui(s_backIconPath, m_rhiFactory);
}

void
GuiContentBrowserWindow::OnDraw() {
  auto& projectDir = AssetRegistry::GetInstance()->GetProjectDir();
  if (!s_currentPath.has_value() || !AssetPath::IsSubpath(*s_currentPath, projectDir)) {
    s_currentPath = projectDir;
  }

  static float thumbnailSize = 64;
  static float thumbnailPadding = 10;
  static float ceilSize = thumbnailSize + thumbnailPadding;

  float panelWidht = ImGui::GetContentRegionAvail().x;
  int columnCount = static_cast<int>(panelWidht / ceilSize);
  columnCount = columnCount <= 0 ? 1 : columnCount;

  ImGui::Columns(columnCount, nullptr, false);

  /**
   * draw directories and files
   */
  if (s_currentPath != projectDir) {
    ImGui::ImageButton(m_backIcon, {thumbnailSize, thumbnailSize});
    if (ImGui::IsItemHovered()) {
      if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) s_currentPath = s_currentPath->parent_path();
    }
    ImGui::NextColumn();
  }

  std::optional<std::filesystem::directory_entry> selectedDir;
  for (auto& entry : std::filesystem::directory_iterator(*s_currentPath)) {
    const auto& entryPath = entry.path();
    auto name = entry.path().filename().string();
    if (entry.is_directory() && name == ".import") {
      continue;
    } else if (name == "assert.db") {
      continue;
    }

    ImGui::PushID(name.c_str());
    if (entry.is_directory()) {
      ImGui::ImageButton(m_floderIcon, {thumbnailSize, thumbnailSize});
      if (ImGui::IsItemHovered()) {
        selectedDir = entry;
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) s_currentPath = entry.path();
      }
    } else {
      ImGui::ImageButton(m_fileIcon, {thumbnailSize, thumbnailSize});
    }

    if (ImGui::BeginDragDropSource()) {
      static AssetPath path;
      // = "res://" + std::filesystem::relative(entry.path(), projectDir).string();
      if (AssetPath::ConvertToAssetPath(entry.path(), path)) {
        ImGui::Text("%s", path.to_string().c_str());
        ImGui::SetDragDropPayload(CONTENT_BROWSER_DRAGDROG, &path, sizeof(path));
        ImGui::EndDragDropSource();
      }
    }

    ImGui::Text("%s", name.c_str());

    ImGui::NextColumn();

    ImGui::PopID();
  }

  /**
   * draw popup menu
   */
  static std::string_view contentBrowserName = "ContentBrowserPopup";
  static std::optional<std::filesystem::directory_entry> s_selectedDir;
  if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
    ImGui::OpenPopup(contentBrowserName.data());
    s_selectedDir = selectedDir;
  }

  static bool isOpenInputFolderNameModal = false;
  if (ImGui::BeginPopup(contentBrowserName.data())) {
    if (ImGui::MenuItem("Create New Folder")) {
      isOpenInputFolderNameModal = true;
    }
    if (s_selectedDir) {
      if (ImGui::MenuItem("Delete Folder")) {
        std::filesystem::remove_all(*s_selectedDir);
        s_selectedDir = std::nullopt;
      }
    }

    ImGui::EndPopup();
  }

  if (isOpenInputFolderNameModal) {
    ImGui::OpenPopup("InputFolderName");
  }

  // Always center this window when appearing
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

  if (ImGui::BeginPopupModal("InputFolderName", &isOpenInputFolderNameModal, ImGuiWindowFlags_AlwaysAutoResize)) {
    static char newFolderName[100] = {0};
    ImGui::InputText("folder name", newFolderName, IM_ARRAYSIZE(newFolderName));

    auto newFolderPath = *s_currentPath / newFolderName;
    if (std::filesystem::exists(newFolderPath)) {
      if (strnlen(newFolderName, IM_ARRAYSIZE(newFolderName)) == 0) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "new directory name is empty");
      } else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "exists directory");
      }
    }

    if (ImGui::Button("OK", ImVec2(120, 0))) {
      if (!std::filesystem::exists(newFolderPath)) {
        std::filesystem::create_directory(newFolderPath);
        isOpenInputFolderNameModal = false;
      }
      ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
      isOpenInputFolderNameModal = false;
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

}  // namespace Marbas::Gui
