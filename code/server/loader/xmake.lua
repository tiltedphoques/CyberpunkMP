target("Server.Loader")
    set_kind("binary")
    set_group("Server")
    add_rules("csharp")
    add_files("Server.Loader.csproj")
    add_extrafiles("**.cs")

    add_deps("Server.Scripting", "Server.Native")

    on_build(function (target, opt)
        import("utils.progress")

        local script = target:scriptdir()
        local proj = path.join(script, "Server.Loader.csproj")
        local mode = is_mode("debug") and "Debug" or "Release"
        progress.show(opt.progress, "${color.build.target}build Server.Loader")
        os.run("dotnet build " .. proj .. " -c " .. mode)
        os.run("dotnet publish " .. proj .. " -c " .. mode .. " -o " .. target:targetdir())
    end)

    after_install(function (target)
        os.cp(path.join(target:installdir("bin"), "Server.Loader.dll"), path.join(target:installdir("launcher"), "server", "Server.Loader.dll"))
        os.cp(path.join(target:installdir("bin"), "Server.Loader.exe"), path.join(target:installdir("launcher"), "server", "Server.Loader.exe"))
        os.cp(path.join(target:installdir("bin"), "Server.Loader.runtimeconfig.json"), path.join(target:installdir("launcher"), "server", "Server.Loader.runtimeconfig.json"))
        os.cp(path.join(target:installdir("bin"), "Server.Loader.deps.json"), path.join(target:installdir("launcher"), "server", "Server.Loader.deps.json"))
    end)

    on_install(function (target)
        local src_plugins = target:targetdir()
        local dest_plugins = target:installdir("bin")

        os.cp(path.join(src_plugins, "Server.Loader.dll"), path.join(target:installdir("bin"), "Server.Loader.dll"))
        os.cp(path.join(src_plugins, "Server.Loader.exe"), path.join(target:installdir("bin"), "Server.Loader.exe"))
        os.cp(path.join(src_plugins, "Server.Loader.runtimeconfig.json"), path.join(target:installdir("bin"), "Server.Loader.runtimeconfig.json"))
        os.cp(path.join(src_plugins, "Server.Loader.deps.json"), path.join(target:installdir("bin"), "Server.Loader.deps.json"))
    end)