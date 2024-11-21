using CyberpunkSdk.Types;

namespace JobSystem
{
    internal class Police : Job
    {
        public static readonly CName Job = new("Police");

        public override CName Name { get { return Job; } }

        public Police(ulong id) : base(id)
        {
        }
        public override void Update(float delta)
        {
            throw new NotImplementedException();
        }

        public override void HeavyUpdate(float delta)
        {
            throw new NotImplementedException();
        }
    }
}
