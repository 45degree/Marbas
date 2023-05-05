---@diagnostic disable: undefined-global

target('Marbas.AssetManager', function()
  set_kind('static')
  set_languages('c11', 'cxx20')

  add_files('$(projectdir)/src/AssetManager/**.cc')
  add_includedirs('$(projectdir)/src')

  if is_mode('debug') then
    add_defines('DEBUG')
  end

  add_deps('Marbas.RHI', 'Marbas.Common', 'async_simple')
  add_packages('glog', 'glfw', 'glm', 'assimp', 'fmt', 'stb', 'cereal')

  if is_plat('windows') then
    add_packages('icu4c')
    add_defines('NOMINMAX')
  elseif is_plat('linux') then
    add_packages('pkgconfig::icu-uc', 'pkgconfig::icu-io', 'pkgconfig::icu-i18n')
  end
end)
