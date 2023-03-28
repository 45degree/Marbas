---@diagnostic disable: undefined-global, undefined-field

add_rules('mode.debug', 'mode.release', 'mode.valgrind')

includes('3rdPart/')

add_requires('glfw 3.3.8')
add_requires('glm 0.9.9+8')
add_requires('glog v0.6.0')
add_requires('abseil 20220623.0')
add_requires('stb 2021.09.10')
add_requires('assimp v5.2.3')
add_requires('fmt 9.1.0')
add_requires('gtest 1.11.0')
add_requires('entt master')
add_requires('nativefiledialog 1.1.6')
add_requires('cereal 1.3.2')

if is_plat('windows') then
  add_requires('icu4c 72.1')
elseif is_plat('linux') then
  add_requires('pkgconfig::icu-uc')
  add_requires('pkgconfig::icu-io')
  add_requires('pkgconfig::icu-i18n')
end

includes('src/Common/')
includes('src/Editor/')
includes('src/Test')
includes('src/Core')
includes('src/AssetManager')
includes('xmake/glslc.lua')

target('Marbas', function()
  set_kind('phony')
  add_deps('Marbas.Core', 'Marbas.RHI', 'Marbas.Common', 'Marbas.Editor', 'Marbas.AssetManager')
end)
