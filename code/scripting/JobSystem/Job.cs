using CyberpunkSdk.Types;

namespace JobSystem
{
    public abstract class Job
    {
        public abstract CName Name { get; }

        public ulong Id { get; private set; }

        public Job(ulong id)
        {
            this.Id = id;
        }

        public abstract void Update(float delta);

        public abstract void HeavyUpdate(float delta);
    }
}
