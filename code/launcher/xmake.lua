target("Launcher")
    set_kind("phony")

    on_install(function (target)
        local function run_command(cmd, args)
            print("Running command: " .. cmd .. " " .. table.concat(args or {}, " "))
            local stdout, stderr = os.iorunv(cmd, args)
        end

        local function run_install(cmd)
            if os.getenv("IS_CI") == "true" then
                run_command(cmd, {"ci"})
            else
                run_command(cmd, {"install"})
            end
        end

        os.cd("code/launcher")

        local npm = is_host("windows") and "npm.cmd" or "npm"

        -- Run npm commands in the root directory
        run_install(npm)
        run_command(npm, {"run", "build"})

        local electron_dir = path.join(os.curdir(), "electron")
        if not os.isdir(electron_dir) then
            raise("Electron directory not found: " .. electron_dir)
        end

        os.cd(electron_dir)

        -- Run npm commands in the electron directory
        run_install(npm)
        run_command(npm, {"run", "build:electron"})

        -- Copy produced files
        local src = path.join(electron_dir, "out", "win-unpacked")
        local dst = path.join("..", "..", "..", target:installdir("launcher"))
        print(string.format("Copying from %s to %s", src, dst))
        if not os.isdir(src) then
            raise("Source directory not found: " .. src)
        end

        os.cp(path.join(src, "*"), dst)

        -- Reset to base directory
        os.cd("../../..")
    end)
