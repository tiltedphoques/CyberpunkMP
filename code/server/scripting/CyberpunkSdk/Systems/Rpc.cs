using CyberpunkSdk.Internal;
using CyberpunkSdk.Types;
using System.Reflection;

namespace CyberpunkSdk.Systems
{
    [AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
    public class ServerRpcAttribute : Attribute
    {
        public ulong Class = 0;
        public ulong Function = 0;
    }

    [AttributeUsage(AttributeTargets.Method, AllowMultiple = false)]
    public class ClientRpcAttribute : Attribute
    {
        public ulong Class = 0;
        public ulong Function = 0;
    }

    public interface IRpcManager
    {
        void Call(ulong playerId, ulong klass, ulong func, IBuffer args);
    }
}
