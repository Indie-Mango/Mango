target("Sandbox")
    set_kind("binary")
    add_files("Source/**.cpp")
    add_deps("Mango-Engine", "Mango-Entry")