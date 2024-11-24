using System.Reflection;
using System.Runtime.InteropServices;
using CyberpunkSdk.Game;
using CyberpunkSdk.Systems;
using Timer = CyberpunkSdk.Game.Timer;

namespace CyberpunkSdk
{
    public static class Server
    {
        static public PlayerSystem PlayerSystem { get; internal set; }
        static public World World { get; internal set; }
        static public Timer Timer { get; internal set; }
        static public IRpcManager RpcManager { get; internal set; }
    }
}
