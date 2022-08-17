---@diagnostic disable: undefined-global

target("Marbas.Tool")
    set_kind("static")
    set_languages("c11", "cxx17")

    add_files("$(projectdir)/src/Tool/**.cc")
    add_includedirs("$(projectdir)/src", { public = true })

    add_packages("glfw", "glm", "glew");
    add_packages("libiconv", "uchardet", "folly", "glog", "openssl");

    add_deps("Marbas.Common")
target_end()
