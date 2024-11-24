using CurseForge.APIClient.Models.Games;
using Cyberpunk.Rpc.Client.Plugins;
using CyberpunkSdk;

namespace Cyberpunk.Rpc.Server.Plugins
{
    public static partial class EmoteServer
    {
        public static partial void TriggerEmote_Impl(ulong playerId, string name)
        {
            var player = CyberpunkSdk.Server.PlayerSystem.GetById(playerId);

            foreach(var id in CyberpunkSdk.Server.PlayerSystem.PlayerIds)
            {
                //if (id == playerId)
                //    continue;

                EmoteClient.TriggerEmote(id, player.PuppetId, name);
            }
        }
    }
}
