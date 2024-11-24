namespace CyberpunkSdk.Game
{
    public abstract class Timer
    {
        public delegate void TickDelegate(float delta);

        public abstract void Add(TickDelegate t, float interval);
        public abstract void Remove(TickDelegate t);
    }
}
