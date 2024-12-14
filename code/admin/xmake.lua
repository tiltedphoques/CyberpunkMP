target("Admin")
    set_kind("phony")
    set_group("Server")

    add_deps("Server.Scripting")

    on_install(function (target)
        local function run_command(cmd, args)
            print("  $ " .. cmd .. " " .. table.concat(args or {}, " "))
            local stdout, stderr = os.iorunv(cmd, args)
        end

        local function run_install(cmd)
            --if os.getenv("IS_CI") == "true" then
            --    run_command(cmd, {"ci"})
            --else
            run_command(cmd, {"install"})
            --end
        end

		print("installing Admin")

        os.cd("code/admin")

        local pnpm = is_host("windows") and "pnpm.cmd" or "pnpm"

        -- Run pnpm commands in the root directory
        run_install(pnpm)
        run_command(pnpm, {"run", "build"})

        local src = path.join(os.curdir(), "dist")
        if not os.isdir(src) then
            raise(string.format("Source directory not found %s", src))
        end

        local dst = path.join("..", "..", target:installdir("launcher"), "server", "assets")
        if os.isdir(dst) then
            os.rmdir(dst)
        end

		os.mkdir(dst)

        -- Copy produced files
        print(string.format("  Copying from %s to %s", src, dst))
        os.cp(path.join(src, "*"), dst)

        -- Reset to base directory
        os.cd("../..")
    end)
