using CyberpunkSdk.Internal;

namespace CyberpunkSdk.Game
{
    public class Player
    {
        private IPlayer player;

        internal Player(IPlayer player)
        {
            this.player = player;
        }

        public void SendChat(string From, string Message)
        {
            player.SendChat(From, Message);
        }

        public ulong Id
        {
            get { return player.Id; }
        }

        public ulong PuppetId
        {
            get {  return player.PuppetId; }
        }

        public uint ConnectionId
        {
            get {  return player.ConnectionId; }
        }

        public Components.MovementComponent MovementComponent
        {
            get { return new Components.MovementComponent(player.MovementComponent); }
        }

        public Components.AttachmentComponent AttachmentComponent
        {
            get { return new Components.AttachmentComponent(player.AttachmentComponent); }
        }

        public string Username
        {
            get { return player.Username; }
        }
    }
}
