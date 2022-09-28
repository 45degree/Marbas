---@diagnostic disable: undefined-global, undefined-field

add_rules("mode.debug", "mode.release")

includes("3rdPart/")

add_requires("glfw 3.3.6")
add_requires("glew 2.2.0")
add_requires("glm 0.9.9+8")
add_requires("glog v0.5.0")
add_requires("folly 2022.04.25")
add_requires("stb 2021.09.10")
add_requires("assimp v5.2.3")
add_requires("toml++ v3.1.0")
add_requires("libiconv 1.16")
add_requires("gtest 1.11.0")
add_requires("entt v3.9.0")
add_requires("nativefiledialog 1.1.6")
add_requires("shaderc")
add_requires("openssl")

if is_plat("linux") then
  add_requires("vulkan")
else
  option("Vulkan SDK Path")
    set_default("D:/VulkanSDK/1.3.224.1")
    set_description("Vulkan SDK Path")
    set_showmenu(true)
  option_end()
end

rule("LoadVulkan")
  on_load(function (target)
    if is_plat("linux") then
      target:add("packages", "vulkan")
    else
      local vulkanIncludePath = path.join('$(Vulkan SDK Path)', 'include')
      local vulkanLibPath = path.join('$(Vulkan SDK Path)', 'lib')
      local vulkanBinPath = path.join('$(Vulkan SDK Path)', 'bin')

      if not os.exists(vulkanIncludePath) then
        if os.exists(path.join('$(Vulkan SDK Path)', 'Include')) then
          vulkanIncludePath = path.join('$(Vulkan SDK Path)', 'Include')
        end
      end

      if os.exists(vulkanBinPath) then
        if os.exists(path.join('$(Vulkan SDK Path)', 'Bin')) then
          vulkanBinPath = path.join('$(Vulkan SDK Path)', 'Bin')
        end
      end

      if os.exists(vulkanLibPath) then
        if os.exists(path.join('$(Vulkan SDK Path)', 'Lib')) then
          vulkanLibPath = path.join('$(Vulkan SDK Path)', 'Lib')
        end
      end

      target:add('includedirs', vulkanIncludePath)
      target:add("links", path.join(vulkanLibPath, "vulkan-1"))
      target:add('rpathdirs', vulkanBinPath)
    end
  end)
rule_end()

includes("src/App/Editor/")
includes("src/App/Games/")
includes("src/Common/")
includes("src/RHI")
includes("src/Tool")
includes("src/Test")
includes("src/Core")
includes("src/Resource")

target("Marbas")
  set_kind("phony")
  add_deps(
    "Marbas.Core",
    "Marbas.RHI",
    "Marbas.Common",
    "Marbas.Editor",
    "Marbas.Resource"
  )
target_end()
