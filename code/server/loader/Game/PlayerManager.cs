using CyberpunkSdk.Game;
using CyberpunkSdk.Internal;

namespace Server.Loader.Game
{
    public class PlayerSystem : CyberpunkSdk.Game.PlayerSystem
    {
        public override IEnumerable<ulong> PlayerIds => _players;

        private readonly List<ulong> _players = [];

        internal PlayerSystem()
        {
        }

        internal void PlayerJoin(ulong Id)
        {
            OnPlayerJoin(Id);
            _players.Add(Id);
        }

        internal void PlayerLeft(ulong Id)
        {
            _players.Remove(Id);
            OnPlayerLeft(Id);
        }

        public override Player GetById(ulong Id)
        {
            return new Player(IPlayerManager.Get().GetPlayer(Id));
        }
    }
}
