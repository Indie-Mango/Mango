set_project("Mango")
set_languages("c++23")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate")

includes("Mango")
includes("Sandbox")