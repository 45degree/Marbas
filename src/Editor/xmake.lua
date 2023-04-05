---@diagnostic disable: undefined-global, undefined-field

target('Marbas.Editor', function()
  set_kind('binary')
  add_files('$(projectdir)/src/Editor/**.cc')
  set_languages('c11', 'cxx20')

  on_load(function()
    local executedir = path.join('$(buildir)', '$(os)', '$(arch)', '$(mode)')

    -- os.cp("$(projectdir)/resource", executedir)
    os.cp('$(projectdir)/assert', executedir)

    -- if not os.exists(path.join(executedir, 'Shader')) then
    --   os.mkdir(path.join(executedir, 'Shader'))
    -- end
    --
    -- os.cp('$(projectdir)/src/Shader/*', path.join(executedir, 'Shader'))
  end)

  add_includedirs('$(projectdir)/src')
  add_deps('Marbas.Core', 'ImGuizmo', 'Marbas.Common', 'Marbas.AssetManager')
  add_packages('glfw', 'fmt', 'assimp', 'glm', 'entt', 'glog', 'nativefiledialog', 'cereal', 'stb')

  if is_plat('windows') then
    add_packages('icu4c')
    add_defines('NOMINMAX')
  elseif is_plat('linux') then
    add_packages('pkgconfig::icu-uc', 'pkgconfig::icu-io', 'pkgconfig::icu-i18n')
  end
end)
