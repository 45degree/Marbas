---@diagnostic disable: undefined-global

target("Marbas.RHI")
  set_kind("static")
  set_languages("c11", "cxx20")

  add_files("$(projectdir)/src/RHI/**.cc")
  add_includedirs("$(projectdir)/src", { public = true })

  -- add_cxxflags("-fPIC", { force = true })

  if is_mode("debug") then
      add_defines("DEBUG")
  end

  add_rules("LoadVulkan")
  add_deps("Imgui")
  add_packages("glfw", "glm", "glog", "glew", "folly", "stb", "shaderc")
target_end()
