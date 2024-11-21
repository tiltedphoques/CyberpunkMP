using CyberpunkSdk.Internal;

namespace CyberpunkSdk.Game
{
    public class PlayerSystem
    {
        public delegate void PlayerEvent(ulong Id);

        public event PlayerEvent? PlayerJoinEvent;
        public event PlayerEvent? PlayerLeftEvent;
        public IEnumerable<ulong> PlayerIds { get { return _players; } }
        private readonly List<ulong> _players = [];

        internal PlayerSystem()
        {
        }

        internal void PlayerJoin(ulong Id)
        {
            PlayerJoinEvent?.Invoke(Id);
            _players.Add(Id);
        }

        internal void PlayerLeft(ulong Id)
        {
            _players.Remove(Id);
            PlayerLeftEvent?.Invoke(Id);
        }

        public Player GetById(ulong Id)
        {
            return new Player(IPlayerManager.Get().GetPlayer(Id));
        }
    }
}
