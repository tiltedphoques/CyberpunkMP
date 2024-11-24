using System.Reflection;
using System.Runtime.InteropServices;
using CyberpunkSdk.Game;
using CyberpunkSdk.Systems;
using Timer = CyberpunkSdk.Game.Timer;

namespace CyberpunkSdk
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

        static public PlayerSystem PlayerSystem => playerSystem;
        static public World World => world;
        static public Timer Timer => timer;
        static public RpcManager RpcManager => rpcManager;

        static Server()
        {
            AppDomain.CurrentDomain.AssemblyResolve += OnAssemblyResolve;

            statistics = new Statistics();
            world = new World();
            rpcManager = new RpcManager(statistics, world);
            playerSystem = new PlayerSystem();
            timer = new Timer(world);

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

            if(File.Exists(path))
            {
                var assembly = Assembly.LoadFrom(path);
                if(assembly != null)
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

        internal static int Update(IntPtr args, int sizeBytes)
        {
            float delta = Marshal.PtrToStructure<float>(args);
            World.Update(delta);
            return 0;
        }

        internal static int PlayerJoin(IntPtr args, int sizeBytes)
        {
            ulong id = Marshal.PtrToStructure<ulong>(args);
            playerSystem.PlayerJoin(id);
            return 0;
        }

        internal static int PlayerLeft(IntPtr args, int sizeBytes)
        {
            ulong id = Marshal.PtrToStructure<ulong>(args);
            playerSystem.PlayerLeft(id);
            return 0;
        }
    }
}
