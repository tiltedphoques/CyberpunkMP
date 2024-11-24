using CyberpunkSdk.Game;

namespace Server.Loader.Game
{
    public class Timer : CyberpunkSdk.Game.Timer
    {
        private class TickInfo
        {
            public TickDelegate Tick;
            public float Interval;
            public float ElapsedTime;

            public TickInfo(TickDelegate tick, float interval)
            {
                Tick = tick;
                Interval = interval;
                ElapsedTime = 0;
            }
        }

        private List<TickInfo> ticks;

        internal Timer(World world)
        {
            ticks = new List<TickInfo>();
            world.UpdateEvent += Update;
        }

        public override void Add(TickDelegate t, float interval)
        {
            ticks.Add(new TickInfo(t, interval));
        }

        public override void Remove(TickDelegate t)
        {
            ticks.RemoveAll(ti => ti.Tick == t);
        }

        internal void Update(float delta)
        {
            for (int i = 0; i < ticks.Count; i++)
            {
                var tickInfo = ticks[i];
                tickInfo.ElapsedTime += delta;

                if (tickInfo.ElapsedTime >= tickInfo.Interval)
                {
                    tickInfo.Tick(tickInfo.ElapsedTime);
                    tickInfo.ElapsedTime = 0;
                }
            }
        }
    }
}
