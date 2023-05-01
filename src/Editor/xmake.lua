---@diagnostic disable: undefined-global, undefined-field

target('Marbas.Editor', function()
  set_kind('binary')
  add_files('$(projectdir)/src/Editor/**.cc')
  set_languages('c11', 'cxx20')

  on_load(function()
    local executedir = path.join('$(buildir)', '$(os)', '$(arch)', '$(mode)')
    os.cp('$(projectdir)/assert', executedir)
  end)

  add_includedirs('$(projectdir)/src')
  add_deps('Marbas.Core', 'ImGuizmo', 'Marbas.Common', 'Marbas.AssetManager')
  add_packages('glfw', 'fmt', 'assimp', 'glm', 'entt', 'glog', 'nativefiledialog', 'cereal', 'stb')

  add_packages('icu4c')
  if is_plat('windows') then
    add_defines('NOMINMAX')
  end
end)
