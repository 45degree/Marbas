---@diagnostic disable: undefined-global, undefined-field

target('Marbas.Test', function()
  set_kind('binary')
  set_languages('c11', 'cxx20')
  add_deps('Marbas.Core', 'Marbas.RHI', 'Marbas.Common', 'Marbas.AssetManager')

  add_includedirs('$(projectdir)/src')
  add_includedirs('$(projectdir)/src/Test')

  -- Test file
  add_files('$(projectdir)/src/Test/SceneTest/*.cc')
  add_files('$(projectdir)/src/Test/*.cc')
  add_files('$(projectdir)/src/Test/RenderGraphTest/*.cc')
  add_files('$(projectdir)/src/Test/AssetManager/*.cc')

  if has_config('CascateCount') then
    add_defines('CASCATE_COUNT=$(CascateCount)')
  end

  if has_config('DirectionLightCount') then
    add_defines('MAX_DIRECTION_LIGHT_COUNT=$(DirectionLightCount)')
  end

  if is_mode('debug') then
    add_defines('DEBUG')
  end

  on_load(function()
    local executedir = path.join('$(buildir)', '$(os)', '$(arch)', '$(mode)')

    if not os.isdir(path.join(executedir, 'Test')) then
      os.mkdir(path.join(executedir, 'Test'))
    end

    os.cp('$(projectdir)/src/Test/*.txt', path.join(executedir, 'Test'))
    os.cp('$(projectdir)/assert/*', path.join(executedir, 'Test'))
  end)

  add_packages('gtest', 'abseil', 'toml++', 'entt', 'glfw', 'glm', 'glog', 'fmt', 'cereal')
end)
