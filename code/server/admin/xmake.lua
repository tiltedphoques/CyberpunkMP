target("Admin")
    set_kind("phony")
    set_group("Server")

    add_deps("Server.Loader", "Server.Native", "Server.Scripting")

    local function get_pnpm(os)
        local function pnpm(args)
            local cmd = is_host("windows") and "pnpm.cmd" or "pnpm"

            print("$ " .. cmd .. " " .. table.concat(args or {}, " "))
            os.runv(cmd, args)
        end

        local function pnpm_install()
            --if os.getenv("IS_CI") == "true" then
            --    pnpm({"ci"})
            --else
            pnpm({"install"})
            --end
        end

        return pnpm, pnpm_install
    end

    local function setup(os, raise, dst)
        os.cd("code/server/admin")

        local pnpm, pnpm_install = get_pnpm(os)

        -- Run pnpm commands in the root directory
        pnpm_install()
        pnpm({"run", "build"})

        local src = path.join(os.curdir(), "dist")
        if not os.isdir(src) then
            raise(string.format("Source directory not found %s", src))
        end

        dst = path.join("..", "..", "..", dst)
        if os.isdir(dst) then
            os.rmdir(dst)
        end
        os.mkdir(dst)

        -- Copy produced files
        print(string.format("Copying from %s to %s", src, dst))
        os.cp(path.join(src, "*"), dst)

        -- Reset to base directory
        os.cd("../../..")
    end

    on_build(function (target, opt)
        import("utils.progress")

        progress.show(opt.progress, "${color.build.target}build Server.Admin")

        local dst = path.join(target:targetdir(), "assets")
        setup(os, raise, dst)
    end)

    on_install(function (target)
        import("utils.progress")

        print("installing Server.Admin")

        local dst = path.join(target:installdir("launcher"), "server", "assets")
        setup(os, raise, dst)
    end)
