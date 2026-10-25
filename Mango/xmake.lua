add_requires("vulkan-hpp")
add_requires("vulkan-loader")
add_requires("vk-bootstrap")
add_requires("shaderc")

add_requires("glfw")

target("Mango-Driver")
    set_kind("static")
    add_files("Driver/Vulkan/**.cpp")
    add_headerfiles("Driver/Vulkan/**.hpp", {install = false})
    add_includedirs(".", {public = true})

    add_packages("vulkan-hpp", "vulkan-loader", "vk-bootstrap", "shaderc", {public = true})
    add_defines("VULKAN_HPP_NO_EXCEPTIONS", {public = true})
    add_defines("GLFW_INCLUDE_VULKAN", {public = true})
    add_packages("glfw", {public = true})

    if is_mode("debug") then
        add_defines("MANGO_DEBUG")
    end

target("Mango-Engine")
    set_kind("static")
    add_files("Mango/**.cpp")
    add_headerfiles("Mango/**.hpp", {install = false})
    add_includedirs(".", {public = true})

    if is_mode("debug") then
        add_defines("MANGO_DEBUG")
    end

    add_deps("Mango-Driver")
