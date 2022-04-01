---@diagnostic disable: undefined-global

target("Marbas.Test")
    set_kind("binary")
    set_languages("c11", "cxx17")

    add_files("$(projectdir)/src/Test/**.cc");
    add_includedirs("$(projectdir)/src")

    if is_mode("debug") then
        add_defines("DEBUG")
    end

    on_load(function ()
        local executedir = path.join("$(buildir)", "$(os)", "$(arch)", "$(mode)")

        if os.exists(path.join(executedir, "Test")) then
            os.rm(path.join(executedir, "Test"))
            os.mkdir(path.join(executedir, "Test"))
        end

        os.cp("$(projectdir)/src/Test/*.txt", path.join(executedir, "Test"))
    end)

    add_deps("Marbas.Tool")
    add_packages("gtest", "folly")
target_end()
