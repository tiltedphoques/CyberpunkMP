if not os.isfile(get_config("game")) then
    print("Please set the path to Cyberpunk2077.exe using the xmake f --game=<path> command")
else
    local function is_game_running(os)
        if not is_plat("windows") then
            return false
        end
        local stdout, stderr = os.iorunv("tasklist", {"/FI", "IMAGENAME eq Cyberpunk2077.exe"})

        return string.find(stdout, "Cyberpunk2077.exe")
    end

    target("Cyberpunk2077")
        set_kind("binary")
        set_basename("Cyberpunk2077")
        set_group("Client")
        add_options("game")
        set_runargs("--online", "--skipStartMenu", "--ip=127.0.0.1")
        set_targetdir(path.directory(get_config("game")))
        on_build(function(target) end)
        on_clean(function(target) end)
        on_link(function(target) end)
        on_install(function(target) end)
        after_build(function(target)
            local targetdir = target:targetdir()
            local clientdir = path.join(os.projectdir(), target:dep("Client"):targetdir())

            local basedir = path.join(targetdir, "..\\..\\")
            local red4extPlugins = path.join(basedir, "red4ext\\plugins")

            local modPath = path.join(red4extPlugins, "zzzCyberpunkMP")

            if is_game_running(os) then
                print("CyberpunkMP.dll not installed: game is running.")
                return
            end
            if os.exists(modPath) then
                if os.isdir(modPath) then
                    os.rmdir(modPath)
                else
                    os.rm("zzzCyberpunkMP")
                end
            end

            os.mkdir(modPath)

            local client_dll_path = path.join(clientdir, "CyberpunkMP.dll")
            local symlink_path = path.join(modPath, "CyberpunkMP.dll")

            os.ln(client_dll_path, symlink_path)
        end)

        add_deps("Client")

    target("RpcGenerator")
        set_kind("binary")
        set_basename("Cyberpunk2077")
        add_options("game", "rpcdir")
        set_group("Client")
        set_runargs("--online", "--skipStartMenu", "--ip=127.0.0.1", "--rpc", "--rpcdir=\"" .. get_config("rpcdir") .. "\"")
        set_targetdir(path.directory(get_config("game")))
        on_build(function(target) end)
        on_clean(function(target) end)
        on_link(function(target) end)
        on_install(function(target) end)
        after_build(function(target)
            local targetdir = target:targetdir()
            local clientdir = path.join(os.projectdir(), target:dep("Client"):targetdir())

            local basedir = path.join(targetdir, "..\\..\\")
            local red4extPlugins = path.join(basedir, "red4ext\\plugins")

            local modPath = path.join(red4extPlugins, "zzzCyberpunkMP")

            if is_game_running(os) then
                print("CyberpunkMP.dll not installed: game is running.")
                return
            end
            if os.exists(modPath) then
                if os.isdir(modPath) then
                    os.rmdir(modPath)
                else
                    os.rm("zzzCyberpunkMP")
                end
            end

            os.mkdir(modPath)

            local client_dll_path = path.join(clientdir, "CyberpunkMP.dll")
            local symlink_path = path.join(modPath, "CyberpunkMP.dll")

            os.ln(client_dll_path, symlink_path)
        end)

        add_deps("Client")
end