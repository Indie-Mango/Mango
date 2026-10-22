add_requires("glfw")

target("Mango-Engine")
    set_kind("static")
    add_files("Mango/**.cpp")
    add_headerfiles("Mango/**.hpp", {install = false})
    add_includedirs(".", {public = true})

    add_packages("glfw", {public = true})

    if is_mode("debug") then
        add_defines("MANGO_DEBUG")
    end
