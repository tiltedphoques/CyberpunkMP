using System.Security;
using CyberpunkSdk.Internal;
using EmbedIO;
using EmbedIO.Actions;
using EmbedIO.Authentication;
using Server.Loader.Extensions;
using Swan.Logging;

namespace Server.Loader.Systems
{
    internal class WebApiPluginDto
    {
        public required string Name { get; set; }
    }

    internal class WebApi
    {
        private Plugins Plugins { get; set; }

        private Statistics Statistics { get; }

        private Task? ServerTask { get; set; }

        private bool Running { get; set; } = true;

        internal WebApi(Plugins plugins, Statistics statistics)
        {
            Statistics = statistics;
            Plugins = plugins;

            var port = IConfig.Get().Port;
            var url = $"http://+:{port}";

            ServerTask = CreateWebServer(url).RunAsync();
        }

        ~WebApi()
        {
            ServerTask?.Dispose();
        }

        private WebServer CreateWebServer(string url)
        {
            Logger.UnregisterLogger<ConsoleLogger>();

            var server = new WebServer(o => o
                .WithUrlPrefix(url)
                .WithMode(HttpListenerMode.EmbedIO))
                .WithModule(new ActionModule("/api/v1/mods/", HttpVerbs.Get, HandleModsRoute))
                .WithModule(new ActionModule("/api/v1/statistics/", HttpVerbs.Get, HandleStatistics));

            RegisterAuthentication(server);
            RegisterPlugins(server);
            RegisterAssets(server);
            return server;
        }

        private void RegisterPlugins(WebServer server)
        {
            var withAssets = Plugins.GetPlugins(p => p.Assets != null);

            foreach (var plugin in withAssets)
            {
                server.WithStaticFolder(
                    baseRoute: plugin.GetAssetsUrl,
                    fileSystemPath: plugin.GetAssetsPath,
                    isImmutable: EnvironmentHelper.IsRelease,
                    configure: m => { m.AddCustomMimeType(".umd.js", "application/javascript"); });
            }
            var withHooks = Plugins.GetPlugins(p => p.WebApi != null);

            foreach (var plugin in withHooks)
            {
                var hook = plugin.WebApi!;
                var controller = hook.BuildController()();

                server.WithWebApi(
                    baseRoute: plugin.GetWebApiUrl,
                    configure: m => m.WithController(controller));
            }

            server.WithModule(new ActionModule("/api/v1/plugins/", HttpVerbs.Get, HandleListPlugins));
        }

        private void RegisterAssets(WebServer server)
        {
            var assetsPath = FileSystemHelper.GetPath("assets");

            server.WithStaticFolder(
                baseRoute: "/",
                fileSystemPath: assetsPath,
                isImmutable: EnvironmentHelper.IsRelease);
        }

        private void RegisterAuthentication(WebServer server)
        {
            if (EnvironmentHelper.IsDebug)
            {
                return;
            }

            // NOTE: should be replaced with Discord authentication later.
            var username = Environment.GetEnvironmentVariable("CYBERPUNKMP_ADMIN_USERNAME");
            var password = Environment.GetEnvironmentVariable("CYBERPUNKMP_ADMIN_PASSWORD");

            if (username == null || password == null)
            {
                throw new SecurityException("You must provide admin credentials using environment variables.");
            }

            // NOTE: always allow access to the following routes for the Server
            //       List System.
            var module = new AdminAuthModule([
                "/api/v1/mods",
                "/api/v1/statistics",
            ]);

            module.WithAccount(username, password);
            server.WithModule(module);
        }

        #region Routes

        private Task HandleModsRoute(IHttpContext context)
        {
            return context.SendDataAsync(new
            {
                Mods = Plugins.ClientMods
            });
        }

        private Task HandleStatistics(IHttpContext context)
        {
            return context.SendDataAsync(new
            {
                Statistics.Rpcs
            });
        }

        private Task HandleListPlugins(IHttpContext context)
        {
            var withHooks = Plugins.GetPlugins(p => p.WebApi != null);

            return context.SendDataAsync(
                withHooks.Select(plugin => new WebApiPluginDto
                    {
                        Name = plugin.Name,
                    })
                    .ToList()
            );
        }

        #endregion
    }

    internal class AdminAuthModule(string[] whitelist) : BasicAuthenticationModule("/", "Admin")
    {
        protected override Task<bool> VerifyCredentialsAsync(string path, string userName, string password,
            CancellationToken cancellationToken)
        {
            if (whitelist.Any(p => p == path))
            {
                return Task.FromResult(true);
            }

            return base.VerifyCredentialsAsync(path, userName, password, cancellationToken);
        }
    }
}