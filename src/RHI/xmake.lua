---@diagnostic disable: undefined-global

target("Marbas.RHI")
    set_kind("static")
    set_languages("c11", "cxx17")

    add_files("$(projectdir)/src/RHI/**.cc")
    add_includedirs("$(projectdir)/src", { public = true })

    if is_mode("debug") then
        add_defines("DEBUG")
    end

    add_packages("glfw", "glm", "glog", "glew", "folly", "stb")
target_end()
