namespace CyberpunkSdk.Types
{
    public class GameTime
    {
        public uint Seconds { get; private set; }

        public GameTime(uint seconds) 
        {
            Seconds = seconds;
        }
    }
}
