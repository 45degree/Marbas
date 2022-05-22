target("Marbas.Common")
  set_kind("static")
  set_languages("c11", "cxx20")

  add_includedirs("$(projectdir)/src")
  add_files("$(projectdir)/src/Common/**.cc")
  add_packages("folly", "assimp", "toml++", "entt", "glm", "glfw")
target_end()
