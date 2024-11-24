set_xmakever("2.8.0")
set_policy("build.ccache", false)
set_policy("package.requires_lock", false)

-- c code will use c99,
set_languages("c99", "cxx20")
add_configfiles("BuildInfo.h.in")

add_requires(
    "mimalloc 2.1.7",
    "spdlog",
    "hopscotch-map",
    "cryptopp",
    "gamenetworkingsockets",
    "glm",
    "openssl",
    "zlib",
    "nlohmann_json",
    "dotnet",
    "flecs v4.0.3",
    "protobuf-cpp",
    "entt",
    "microsoft-gsl")

if is_plat("windows") then
    set_arch("x64")
    add_cxflags("/bigobj")
    add_defines("NOMINMAX")
end

add_defines("_UNICODE", "RED4EXT_STATIC_LIB", "GLM_ENABLE_EXPERIMENTAL")

set_warnings("all")
add_vectorexts("sse", "sse2", "sse3", "ssse3")

-- build configurations
add_rules("mode.debug", "mode.releasedbg", "mode.release")

if is_mode("debug") then
    add_defines("TP_DEBUG")
    set_symbols("debug", "edit")
end

includes('tools/codegen')

-- add projects
includes("code/netpack")
includes("code/common")
includes("code/protocol")
includes("code/server")
includes("code/scripting")

if is_plat("windows") then
    includes("code/assets")
    includes("code/client")
    includes("code/launcher")
    includes("vendor/")

    includes("@builtin/xpack")

    xpack("Cyberpunk Multiplayer")
        set_formats("zip")
        set_title("Cyberpunk Multiplayer")
        set_basename("Artifacts")
        set_author("Tilted Phoques SRL")
        set_description("Installer for Cyberpunk Multiplayer Launcher")
        --set_homepage("https://your-project-homepage.com")
        --set_licensefile("LICENSE.md")  -- Make sure this file exists in your project
        local function add_files_recursively(dir, root_dir)
            local relative_path = path.relative(dir, root_dir)
            add_installfiles(path.join(dir, "*"), {prefixdir = relative_path})
            for _, subdir in ipairs(os.dirs(path.join(dir, "*"))) do
                add_files_recursively(subdir, root_dir)
            end
        end
    
        add_files_recursively("distrib/launcher")

        set_version("0.1.0.0")
end
