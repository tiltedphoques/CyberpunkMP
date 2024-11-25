using CyberpunkSdk.Internal;

namespace CyberpunkSdk.Game
{
    public abstract class PlayerSystem
    {
        public delegate void PlayerEvent(ulong Id);

        public event PlayerEvent? PlayerJoinEvent;
        public event PlayerEvent? PlayerLeftEvent;
        public abstract IEnumerable<ulong> PlayerIds { get; }

        public abstract Player GetById(ulong Id);

        protected void OnPlayerJoin(ulong Id)
        {
            PlayerJoinEvent?.Invoke(Id);
        }

        protected void OnPlayerLeft(ulong Id)
        {
            PlayerLeftEvent?.Invoke(Id);
        }
    }
}
