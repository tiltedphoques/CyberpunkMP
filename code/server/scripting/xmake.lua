target("Server.Scripting")
    set_kind("phony")
    set_group("Server")
    add_extrafiles("SdkGenerator/*.cs")
    add_extrafiles("../native/Scripting/I*.h")

    set_policy('build.fence', true)

    local function dirname(path)
        path = path:gsub("[\\/]+$", "")
        return path:match("[^\\/]+$")
    end

    local function get_plugins(root, exclude)
        local function is_excluded(dir)
            for _, path in ipairs(exclude) do
                if dir:match(path .. "$") then
                    return true
                end
            end
            return false
        end
        local plugins = {}

        root = path.join(root, "*")
        for _, dir in ipairs(os.dirs(root)) do
            local proj = path.join(dir, dirname(dir) .. ".csproj")

            if dir:match("System$") and os.exists(proj) and not is_excluded(dir, exclude) then
                table.insert(plugins, dirname(dir))
            end
        end
        return plugins
    end

    on_build(function (target, opt)
        import("core.project.depend")
        import("utils.progress")

        local dependfile = target:dependfile("scripting")

        --depend.on_changed(function ()
            local output = path.join(target:targetdir(), "plugins")
            local sdk_output = path.join(target:targetdir(), "sdk")
            local script = target:scriptdir()
            local sdk_gen_proj = path.join(script, "SdkGenerator", "SdkGenerator.csproj")
            local sdk_proj = path.join(script, "CyberpunkSdk", "CyberpunkSdk.csproj")

            local files = ""
            for _, file in ipairs(target:extrafiles()) do
                if file:endswith(".h") then
                    files = files .. " " .. file
                end
            end

            -- Build SDK
            progress.show(opt.progress, "${color.build.target}build codegen")
            os.run("dotnet build " .. sdk_gen_proj)
            progress.show(opt.progress, "${color.build.target}codegen sdk")
            os.run("dotnet run --project " .. sdk_gen_proj .. " -- " .. files)
            progress.show(opt.progress, "${color.build.target}build sdk")
            os.run("dotnet publish " .. sdk_proj .. " -o " .. sdk_output)

            -- Build plugins
            local mode = is_mode("debug") and "Debug" or "Release"
            local exclude = {"VehicleSystem"}
            local plugins = get_plugins(script, exclude)

            for _, name in ipairs(plugins) do
                local proj = path.join(script, name, name .. ".csproj")
                local label = string.lower(name:sub(1, -7))

                progress.show(opt.progress, "${color.build.target}build " .. label)
                os.run("dotnet publish " .. proj .. " -c " .. mode .. " -o " .. path.join(output, name))
            end
	--[[	end,
		{
			dependfile = dependfile,
			files = target:extrafiles(),
			changed = target:is_rebuilt()
		})]]--
    end)

    on_install(function (target)
        local src_plugins = path.join(target:targetdir(), "plugins")
        local dest_plugins = path.join(target:installdir("launcher"), "server", "plugins")

        print("Installing plugins from " .. src_plugins .. " to " .. dest_plugins)

        if os.isdir(dest_plugins) then
            os.rmdir(dest_plugins)
        end

        os.cp(src_plugins, dest_plugins)
        os.cp(path.join(target:targetdir(), "sdk", "*"), path.join(target:installdir("launcher"), "server"))

        -- Iterate over subdirectories in the destination plugins folder
        for _, dir in ipairs(os.dirs(path.join(dest_plugins, "*"))) do
            local dir_name = path.basename(dir)

            -- Get all files in the current subdirectory
            local files = os.files(path.join(dir, "*"))

            -- Iterate over files and delete those that don't match the directory name
            for _, file in ipairs(files) do
                local file_name = path.basename(file)
                local file_name_without_ext = path.basename(file):gsub("%..*$", "")
                local file_ext = path.extension(file)

                if file_name_without_ext ~= dir_name or file_ext == ".pdb" then
                    os.rm(file)
                end
            end
        end
    end)
