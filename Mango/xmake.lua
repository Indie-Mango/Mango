target("Mango-Engine")
    set_kind("static")
    add_files("Source/**.cpp")
    add_includedirs("Source", {public = true})

target("Mango-Entry")
    set_kind("object")
    add_files("Source/Main.cpp")
    add_includedirs("Source", {public = true})
