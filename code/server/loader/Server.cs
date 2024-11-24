using System.Reflection;
using Server.Loader.Game;
using Server.Loader.Systems;
using Timer = Server.Loader.Game.Timer;

namespace Server.Loader
{
    public class Server
    {
        static Plugins plugins;
        static WebApi webApi;
        static RpcManager rpcManager;
        static Statistics statistics;
        static PlayerSystem playerSystem;
        static World world;
        static Timer timer;

        static Server()
        {
            AppDomain.CurrentDomain.AssemblyResolve += OnAssemblyResolve;

            statistics = new Statistics();
            world = new World();
            rpcManager = new RpcManager(statistics, world);
            playerSystem = new PlayerSystem();
            timer = new Timer(world);

            CyberpunkSdk.Server.World = world;
            CyberpunkSdk.Server.PlayerSystem = playerSystem;
            CyberpunkSdk.Server.Timer = timer;
            CyberpunkSdk.Server.RpcManager = rpcManager;

            // As late as possible
            plugins = new Plugins(rpcManager);

            webApi = new WebApi(plugins, statistics);
        }

        static internal Assembly? OnAssemblyResolve(object? sender, ResolveEventArgs args)
        {
            var name = new AssemblyName(args.Name).Name;

            if (name == "CyberpunkSdk")
                return typeof(Server).Assembly;

            string downDir = Path.GetDirectoryName(args.RequestingAssembly!.Location)!;
            string path = Path.Combine(downDir, name + ".dll");

            if (File.Exists(path))
            {
                var assembly = Assembly.LoadFrom(path);
                if (assembly != null)
                {
                    return assembly;
                }
            }

            downDir = Directory.GetParent(downDir)!.FullName;
            path = Path.Combine(downDir, name + ".dll");

            if (File.Exists(path))
            {
                var assembly = Assembly.LoadFrom(path);
                if (assembly != null)
                {
                    return assembly;
                }
            }

            return null;
        }

        internal static void Update(float delta)
        {
            world.Update(delta);
        }

        internal static void PlayerJoin(ulong id)
        {
            playerSystem.PlayerJoin(id);
        }

        internal static void PlayerLeft(ulong id)
        {
            playerSystem.PlayerLeft(id);
        }
    }
}
