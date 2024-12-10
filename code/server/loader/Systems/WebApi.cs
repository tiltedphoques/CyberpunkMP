using CyberpunkSdk.Internal;
using EmbedIO;
using EmbedIO.Actions;
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

        private WebServer CreateWebServer(string url)
        {
            Logger.UnregisterLogger<ConsoleLogger>();

            var server = new WebServer(o => o
                .WithUrlPrefix(url)
                .WithMode(HttpListenerMode.EmbedIO))
                .WithModule(new ActionModule("/api/v1/mods/", HttpVerbs.Get, HandleModsRoute))
                .WithModule(new ActionModule("/api/v1/statistics/", HttpVerbs.Get, HandleStatistics));

            RegisterAssets(server);
            RegisterHooks(server);
            // NOTE: must be registered after hooks.
            server.WithModule(new ActionModule("/api/v1/plugins/", HttpVerbs.Get, HandleListPlugins));
            return server;
        }

        private void RegisterHooks(WebServer server)
        {
            foreach (var (name, hook) in Plugins.Hooks)
            {
                var controller = hook.BuildController()();

                server.WithWebApi(
                    baseRoute: $"/api/v1/plugins/{name}",
                    configure: m => m.WithController(controller));
            }
        }

        private void RegisterAssets(WebServer server)
        {
            foreach (var (path, name) in Plugins.Assets)
            {
                server.WithStaticFolder(
                    baseRoute: $"/api/v1/plugins/{name}/assets/",
                    fileSystemPath: path,
#if DEBUG
                    isImmutable: false,
#else
                    isImmutable: true,
#endif
                    configure: m => { m.AddCustomMimeType(".umd.js", "application/javascript"); });
            }
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
            return context.SendDataAsync(
                Plugins.Hooks
                    .Select(hook => new WebApiPluginDto
                    {
                        Name = hook.Key
                    })
                    .ToList()
            );
        }

        #endregion
    }
}
