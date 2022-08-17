---@diagnostic disable: undefined-global

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

-- add_requires("glslang", { configs = { binaryonly = true }})

-- add_requires("uchardet")

-- if is_mode("debug") then
--   add_defines("DEBUG")
-- end

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
