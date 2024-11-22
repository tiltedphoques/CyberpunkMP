target("Scripting")
    set_kind("phony")
    add_extrafiles("SdkGenerator/*.cs")
    add_extrafiles("../server/Scripting/I*.h")

    before_build(function (target, opt)
        import("core.project.depend")
        import("utils.progress")

        local dependfile = target:dependfile("scripting")

        --depend.on_changed(function ()
			local output = path.join(target:targetdir(), "plugins")
            local script = target:scriptdir()
            local sdk_gen_proj = path.join(script, "SdkGenerator", "SdkGenerator.csproj")
            local sdk_proj = path.join(script, "CyberpunkSdk", "CyberpunkSdk.csproj")
            local emote_proj = path.join(script, "EmoteSystem", "EmoteSystem.csproj")
            local job_proj = path.join(script, "JobSystem", "JobSystem.csproj")

            local files = ""
            for _, file in ipairs(target:extrafiles()) do
                if file:endswith(".h") then
                    files = files .. " " .. file
                end
            end

            progress.show(opt.progress, "${color.build.target}build codegen")
            os.run("dotnet build " .. sdk_gen_proj)
            progress.show(opt.progress, "${color.build.target}codegen sdk")
            os.run("dotnet run --project " .. sdk_gen_proj .. " -- " .. files)
            progress.show(opt.progress, "${color.build.target}build sdk")
            os.run("dotnet publish " .. sdk_proj .. " -o " .. output)
            progress.show(opt.progress, "${color.build.target}build emote")
            os.run("dotnet publish " .. emote_proj .. " -o " .. path.join(output, "EmoteSystem"))
            progress.show(opt.progress, "${color.build.target}build job")
            os.run("dotnet publish " .. job_proj .. " -o " .. path.join(output, "JobSystem"))
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