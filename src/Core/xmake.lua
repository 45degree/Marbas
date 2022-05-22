target("Marbas.Core")
  set_kind("static")
  set_languages("c11", "cxx20")

  add_rules("utils.glsl2spv", {
      outputdir = path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)", "shader")
  })

  add_includedirs("$(projectdir)/src")
  add_files("$(projectdir)/src/Core/**.cc")
  -- add_files("$(projectdir)/src/**.glsl")

  add_deps("Marbas.RHI", "Marbas.Tool", "Marbas.Resource", "Marbas.Common",
           "IconFontCppHeaders", "ImGuizmo", "nameof")

  add_packages("glfw", "glm", "glog", "folly", "assimp", "glslang", -- "uchardet",
               "toml++", "entt")

target_end()
