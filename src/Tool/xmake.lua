---@diagnostic disable: undefined-global

target("Marbas.Tool")
    set_kind("static")
    set_languages("c11", "cxx17")

    add_files("$(projectdir)/src/Tool/**.cc")
    add_includedirs("$(projectdir)/src", { public = true })

    if is_mode("debug") then
        add_defines("DEBUG")
    end

    add_packages("glfw", "glm", "glew");
    add_packages("libiconv", "uchardet", "folly", "glog");
target_end()
